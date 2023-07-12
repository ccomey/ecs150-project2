# Multithreading Library

## Overview
This is a simple multithreading library that uses semaphores and preemption.
It uses a custom queue structure to store the threads in FIFO order.

### queue.c
The file *queue.c* specifies a queue struct, containing an int *size*, and
two node structs, *front* and *back*. Each node consists of a void pointer
*data*, which contains the actual data to be held by the queue, and two more
node pointers, *ahead* and *behind*, which store the node's neighbors in the
queue.

In *queue_enqueue()*, the new data is placed in a node, which is placed to the
back of the queue. Likewise, *queue_dequeue()* simply removes and returns the
queue's *front*, while *queue_peek()* returns the front without removing it.

The function *queue_delete* uses each node's *behind* pointer to iterate
through the queue. If it finds the desired data, it deletes it after, then uses
a copy of the data to reset the queue's and the old node's neighbor's pointers.

The function *queue_iterate* iterates through the queue similarly to
*queue_delete*, running the passed-in function on every element until the back
is reached.

Finally, *queue_move_front_to_back()* takes in only the queue, and uses 
*queue_dequeue()* and *queue_enqueue()* to move the front element to the back.
This is used in uthread functions to iterate through queues to find and return
a certain element, like the running thread.

### queue_tester.c
This attempts to cover various cases where *queue.c* could return an error.
It adds elements of varying data types to the queue, iterates through the queue
while deleting elements, dequeues elements that have already been dequeued,
and dequeues elements that were never in the queue. Our final version of the
queue passes all of these checks.

### uthread.c
The *uthread.c* implementation uses a single queue of threads as a global
variable, as it needs to be accessed by every function in the file, which
don't take in a queue as a parameter. The *threads* queue consists of
*uthread_tcb* structs, which contain a stack pointer, a context, the trhead's
function and arguments, and its state, an enum equaling RUNNING, READY,
BLOCKED, or ZOMBIE.

The function *uthread_current* iterates through the threads, from front to
back, and returns the thread with the RUNNING state. It does not exit the
function immediately, and instead continues through the rest of the queue.
This is so the thread order is not interrupted. *queue_iterate* cannot be used
because functions using it cannot have a return value. *find_thread_with_state*
is a more generalized function, taking in a state enum, which searches for the
first thread in the queue with the desired state. This function is frequently
use this to find the next READY thread.

*uthread_yield* iterates through the queue until it finds the running thread.
It deletes it and re-adds it to the queue, effectively moving it to the back.
It states the thread's state to READY and finds another READY thread.
*find_thread_with_state(READY)* is used to find the first ready thread in the
queue. Because that function moves from front to back, the old thread is always
the lowest-priority for being selected, and will only be selected if there is
no other ready thread. *uthread_yield* will then set the new thread's state to
RUNNING and switch to its context.

*uthread_exit* finds the next ready thread and sets its state to RUNNING. It
then finds the running thread, sets its state to ZOMBIE, before switching
contexts from the exiting thread's to the new running thread's.

*uthread_create* creates a new thread, fills out its information, like its
stack pointer and context, and adds it to the queue as a ready thread.

*uthread_run* creates the thread queue, and enters an infinite loop, in which
it finds the next ready thread and starts running it, before running
*uthread_exit* on it. The loop will be broken out of once there are no more
ready threads. At this point, every zombie thread will be freed, the queue will
be deleted, and the program will end.

*uthread_unblock* simply switches the passed-in thread's state from BLOCKED to
READY. *uthread_block* will block the running thread, then run the next ready
thread and switch contexts to it.


### sem.c
This file contains a *semaphore* struct containing an access count and a queue
of blocked threads attempted to access its resource.

*sem_create* and *sem_destroy* are basic create and destroy functions, similar
to those in *queue.c*. *sem_down* blocks the running thread if the access count
is zero and adds the thread to the blocked queue for the semaphore. Otherwise,
it decrements the access count. *sem_up* is the reverse, incrementing the
access count, then dequeuing the front blocked thread for that resource and
unblocking it.


### preempt.c
This file contains a sigaction struct and an itimerval struct. The sigaction 
is to set what happens when SIGVTALRM is sent and and itimerval is to create
the alarm that sends SIGVTALRM. 

*Sighandler* gets called whenever the alarm rings and yields. 
*preempt_disable* and *preempt_enable* turn off and on preemtion by blocking
the SIGVTALRM by using pthread_sigmask. *preempt_start* gets call by
*uthread_run* and either starts if true is passed or not if false. It creates
the alarm and sets the sighandler through *sigaction*. *preempt_stop* disables
the timer and restores the previous signal action, which is run at the end of
*uthread_run*.


### preempt_tester.c
This file creates 6 threads. Thread 1-2 checks *uthread_run* by itself.
Threads 3-4 checks for disable. Threads 5-6 checks for enable. There are
3 *uthread_runs* to test this. The first run interupts periodically
when if would just sleep otherwise. The second disables, and no 
interuptions are seen. The third test enable by first disabling then 
enabling to see if it runs. 
