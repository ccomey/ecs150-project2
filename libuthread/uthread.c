#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

queue_t threads;

struct uthread_tcb {
	/* TODO Phase 2 */
	void* stack_pointer;
	uthread_ctx_t* context;
	enum states{RUNNING, READY, BLOCKED, ZOMBIE} state;
	uthread_func_t func;
	void* args;
};

void print_tcb(queue_t queue, void* data){
	struct uthread_tcb* tcb = (struct uthread_tcb*)data;

	if (tcb == NULL){
		printf("TCB: NULL\n");
		return;
	}

	printf("TCB node pointer: %p\n", tcb);
	printf("\tTCB func pointer: %p\n", tcb->func);
	printf("\tTCB stack pointer: %p\n", tcb->stack_pointer);

	switch (tcb->state){
		case 0:
			printf("\tTCB state: RUNNING\n");
			break;
		
		case 1:
			printf("\tTCB state: READY\n");
			break;

		case 2:
			printf("\tTCB state: BLOCKED\n");
			break;

		case 3:
			printf("\tTCB state: ZOMBIE\n");
			break;

		default:
			printf("\tTCB somehow has an impossible state\n");
			break;
	}
}

void free_zombie_threads(queue_t queue, void* data){
	struct uthread_tcb* tcb = (struct uthread_tcb*)data;

	if (tcb == NULL){
		printf("found null tcb in threads\n");
		return;
	}

	if (tcb->state != ZOMBIE){
		return;
	}

	printf("queue length before: %d\n", queue_length(queue));
	queue_iterate(queue, print_tcb);
	uthread_ctx_destroy_stack(tcb->stack_pointer);
	printf("freed stack\n");
	free(tcb->context);
	printf("freed context\n");
	free(tcb);
	printf("freed element\n");
	queue_delete(queue, data);
	printf("queue length after: %d\n", queue_length(queue));
}

// struct uthread_tcb *uthread_current(void)
// {
// 	/* TODO Phase 2/3 */
// 	struct uthread_tcb* running_thread = NULL;
// 	// printf("uthread_current : queue length = %d\n", queue_length(threads));
// 	for (int i = 0; i < queue_length(threads); i++){
// 		// printf("uthread_current : about to declare t\n");
// 		void* t;
// 		// printf("uthread_current : about to peek\n");
// 		queue_peek(threads, &t);
// 		// printf("uthread_current : t initialized\n");
// 		struct uthread_tcb* checked_thread = (struct uthread_tcb*)t;
// 		// printf("uthread_current : checked_thread initialized\n");
// 		if (checked_thread->state == RUNNING){
// 			// printf("uthread_current : found running thread\n");
// 			running_thread = checked_thread;
// 		} else {
// 			// printf("uthread_current : moving element to back of queue\n");
// 			queue_move_front_to_back(threads);
// 			// printf("uthread_current : moved element to back of queue\n");
// 		}
// 	}

// 	// printf("uthread_current : exited loop\n");
// 	return running_thread;
// }

// finds the RUNNING thread
struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
	struct uthread_tcb* running_thread = NULL;

	// iterate through the threads, from front to back
	for (int i = 0; i < queue_length(threads); i++){

		// examine the front of the queue
		void* t;
		queue_peek(threads, &t);
		struct uthread_tcb* checked_thread = (struct uthread_tcb*)t;

		// save it if it is running
		if (checked_thread->state == RUNNING){
			running_thread = checked_thread;
		}
		
		// move it to the back and check the next item
		queue_move_front_to_back(threads);
	}

	return running_thread;
}

// more generalized version of above
struct uthread_tcb* find_thread_with_state(enum states state){
	struct uthread_tcb* desired_thread = NULL;
	// printf("length in FTWS = %d\n", queue_length(threads));
	// queue_iterate(threads, print_tcb);
	for (int i = 0; i < queue_length(threads); i++){
		void* t;
		queue_peek(threads, &t);
		struct uthread_tcb* checked_thread = (struct uthread_tcb*)t;
		if (desired_thread == NULL && checked_thread->state == state){
			desired_thread = checked_thread;
		}
		
		// move it to the back and check the next item
		queue_move_front_to_back(threads);
	}

	return desired_thread;
}

// void uthread_yield(void)
// {
// 	/* TODO Phase 2 */
// 	// printf("\nin uthread_yield\n");
// 	struct uthread_tcb* running_thread;
// 	for (int i = 0; i < queue_length(threads); i++){
// 		void* t;
// 		queue_peek(threads, &t);
// 		// printf("uthread_yield : initialized t\n");
// 		struct uthread_tcb* checked_thread = (struct uthread_tcb*)t;
// 		// printf("uthread_yield : initialized checked_thread\n");
// 		if (checked_thread->state == RUNNING){
// 			// printf("\nuthread_yield : found running thread: %p\n", checked_thread->func);
// 			running_thread = checked_thread;
// 			// printf("uthread_yield : about to delete checked thread\n");
// 			queue_delete(threads, checked_thread);
// 			// printf("uthread_yield : deleted checked thread\n");

// 			// printf("uthread_yield : about to set state to ready\n");
// 			running_thread->state = READY;
// 			// printf("uthread_yield : set state to ready\n");
// 			queue_enqueue(threads, running_thread);
// 			break;
// 		} else {find_thread_with_state(threads, 
// 			queue_move_front_to_back(threads);
// 		}
// 	}

// 	// printf("threads after uthread_yield: %d total\n", queue_length(threads));
// 	// queue_iterate(threads, print_tcb);

// 	struct uthread_tcb* next_thread = find_thread_with_state(READY);
// 	next_thread->state = RUNNING;
// 	// printf("about to switch contexts\n");
// 	uthread_ctx_switch(running_thread->context, next_thread->context);
// 	// printf("switched contexts\n");
// }

void uthread_yield(void)
{
	/* TODO Phase 2 */

	// uthread_yield finds the running thread and sets it to READY
	struct uthread_tcb* running_thread;
	
	// we will look at every element of the queue, from front to back
	for (int i = 0; i < queue_length(threads); i++){

		// checked_thread is the thread at the front of the queue
		void* t;
		queue_peek(threads, &t);
		struct uthread_tcb* checked_thread = (struct uthread_tcb*)t;

		// printf("before if\n");

		// if it is the running thread, flag it as READY
		if (checked_thread->state == RUNNING){
			running_thread = checked_thread;
			// queue_delete(threads, checked_thread);
			running_thread->state = READY;
			// queue_enqueue(threads, running_thread);
			// printf("chaning running\n");
		} 
		
		// if we don't find the running thread, we move the front element to the back and continue checking
		
		queue_move_front_to_back(threads);
			// printf("else statement\n");
	}

	// find the next READY thread, start it running, and switch contexts to it
	// this will resume the thread where it left off
	struct uthread_tcb* next_thread = find_thread_with_state(READY);
	next_thread->state = RUNNING;

	// printf("\nqueue after yield\n");
	// queue_iterate(threads, print_tcb);
	uthread_ctx_switch(running_thread->context, next_thread->context);
}

void uthread_exit(void)
{
	// printf("inside exit: queue before deletion\n");
	// queue_iterate(threads, print_tcb);
	/* TODO Phase 2 */

	// find the running thread and delete it from the queue
	struct uthread_tcb* exiting_thread = uthread_current();
	// printf("uthread current = %p\n", exiting_thread->func);

	// printf("delete\n");

	// we should only switch contexts if there is another ready thread to switch contexts to
	bool should_switch_contexts = false;
	uthread_ctx_t* context_backup;

	if (queue_length(threads) > 0){
		struct uthread_tcb* next_thread = find_thread_with_state(READY);


		if (next_thread != NULL){
			// printf("found next thread %p\n", next_thread->func);
			// get ready to run the next thread
			
			// printf("about to print thread queue\n");
			// queue_iterate(threads, print_tcb);
			// printf("finished queue printing\n");
			next_thread->state = RUNNING;
			should_switch_contexts = true;
			context_backup = next_thread->context;
		}
	}

	// destroy the thread
	// printf("finished setting up next thread\n");

	// uthread_ctx_destroy_stack(exiting_thread->stack_pointer);
	// printf("stack destroyed\n");
	// free(exiting_thread->context);
	// printf("context freed\n");
	// free(exiting_thread);
	// printf("thread freed\n");
	exiting_thread->state = ZOMBIE;

	// queue_iterate(threads, print_tcb);

	// switch contexts if necessary
	if (should_switch_contexts){
		printf("about to switch contexts in uthread_exit\n");
		uthread_ctx_switch(exiting_thread->context, context_backup);
		// printf("switched contexts\n");
	} else if (uthread_current() == NULL){
		printf("about to switch contexts to a zombie\n");
		void* t;
		queue_peek(threads, &t);
		struct uthread_tcb* zombie_thread = (struct uthread_tcb*)t;
		
		uthread_ctx_switch(exiting_thread->context, zombie_thread->context);
	}
}

/*
 * uthread_create - Create a new thread
 * @func: Function to be executed by the thread
 * @arg: Argument to be passed to the thread
 *
 * This function creates a new thread running the function @func to which
 * argument @arg is passed.
 *
 * Return: 0 in case of success, -1 in case of failure (e.g., memory allocation,
 * context creation).
 */

int uthread_create(uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */
	// printf("in uthread_create\n");
	struct uthread_tcb* new_thread = malloc(sizeof(struct uthread_tcb));
	// printf("malloced new thread\n");
	new_thread->stack_pointer = uthread_ctx_alloc_stack();
	// printf("created stack pointer\n");
	new_thread->context = malloc(sizeof(uthread_ctx_t));

	if (uthread_ctx_init(new_thread->context, new_thread->stack_pointer, func, arg) == -1){
		fprintf(stderr, "error creating context in uthread_create: context likely already exists\n");
		return -1;
	}

	// printf("created context\n");
	
	new_thread->func = func;
	new_thread->args = arg;
	new_thread->state = READY;

	// printf("set up other vars\n");

	queue_enqueue(threads, new_thread);

	// printf("put in queue: length now %d\n", queue_length(threads));

	printf("\nthreads after uthread_create: %d total\n", queue_length(threads));
	queue_iterate(threads, print_tcb);

	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */

	threads = queue_create();
	// printf("uthread_run ran\n");

	// register incoming function as current thread
	if (uthread_create(func, arg)){
		return -1;
	}
	// printf("queue length after creating initial thread: %d\n", queue_length(threads));

	// uthread_ctx_bootstrap(func, arg);

	while (find_thread_with_state(READY) != NULL){
		struct uthread_tcb* ready_thread = find_thread_with_state(READY);
		// printf("found ready thread: %p\n", ready_thread->func);
		ready_thread->state = RUNNING;
		// printf("about to run thread func\n");
		ready_thread->func(ready_thread->args);
		// printf("function finished\n\n");
		// printf("%d\n", ready_thread->state);
		// ready_thread->state = ZOMBIE;
		// printf("zombified thread\n");
		printf("finished running\n");
		uthread_exit();
	}

	printf("about to clear threads\n");
	queue_iterate(threads, print_tcb);
	// for (int i = 0; i < queue_length(threads); i++){

	// 	// save it if it is running
	// 	if (checked_thread->state == ZOMBIE){
	// 		uthread_ctx_destroy_stack(checked_thread->stack_pointer);
	// 		// printf("stack destroyed\n");
	// 		free(checked_thread->context);
	// 		// printf("context freed\n");
	// 		free(checked_thread);
	// 		// printf("thread freed\n");
	// 		queue_delete(threads, checked_thread);
	// 		printf("queue length after finishing = %d\n", queue_length(threads));
	// 	}
	// }

	
	
	// for (struct uthread_tcb* checked_thread = find_thread_with_state(ZOMBIE); checked_thread != NULL; checked_thread = find_thread_with_state(ZOMBIE)){
	// 	printf("clearing thread");
	// 	uthread_ctx_destroy_stack(checked_thread->stack_pointer);
	// 	// printf("stack destroyed\n");
	// 	free(checked_thread->context);
	// 	// printf("context freed\n");
	// 	free(checked_thread);
	// 	// printf("thread freed\n");
	// 	queue_dequeue(threads, (void**)checked_thread);
	// 	// printf("queue length after finishing = %d\n", queue_length(threads));
	// }

	// struct uthread_tcb* checked_thread = find_thread_with_state(ZOMBIE);
	// while (checked_thread != NULL){
	// 	printf("zombie thread = %p\n", checked_thread->func);
	// 	queue_delete(threads, checked_thread);
	// 	printf("removed thread %p\n", checked_thread->func);
	// 	printf("queue length after finishing = %d\n", queue_length(threads));
	// 	printf("queue now:\n");
	// 	queue_iterate(threads, print_tcb);
	// 	free(checked_thread->context);
	// 	printf("context freed\n");
	// 	uthread_ctx_destroy_stack(checked_thread->stack_pointer);
	// 	printf("stack destroyed\n");
	// 	// free(checked_thread);
	// 	// printf("thread freed\n");
	// 	checked_thread = find_thread_with_state(ZOMBIE);
	// }

	queue_iterate(threads, free_zombie_threads);

	if (queue_destroy(threads) == -1){
		fprintf(stderr, "error: threads queue was not entirely cleared\n");
	}

	return 0;
}

void uthread_block(void)
{
	/* TODO Phase 3 */

	// printf("running uthread_block\n");
	struct uthread_tcb* running_thread = find_thread_with_state(RUNNING);
	running_thread->state = BLOCKED;

	// printf("blocked running thread\n");

	struct uthread_tcb* next_thread = find_thread_with_state(READY);
	next_thread->state = RUNNING;
	// printf("enabled next ready thread: about to switch contexts\n");
	// printf("queue after block\n");
	// queue_iterate(threads, print_tcb);
	uthread_ctx_switch(running_thread->context, next_thread->context);
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
	// printf("running uthread_unblock on thread with state %d\n", uthread->state);
	if (uthread != NULL && uthread->state == BLOCKED){
		uthread->state = READY;
	// printf("state switched from BLOCKED to READY\n");
	}
	// printf("queue after unblock\n");
	// queue_iterate(threads, print_tcb);
}