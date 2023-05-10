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
	printf("TCB func: %p\n", tcb->func);

	switch (tcb->state){
		case 0:
			printf("TCB state: RUNNING\n");
			break;
		
		case 1:
			printf("TCB state: READY\n");
			break;

		case 2:
			printf("TCB state: BLOCKED\n");
			break;

		case 3:
			printf("TCB state: ZOMBIE\n");
			break;

		default:
			printf("state error\n");
			break;
	}
}

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
	struct uthread_tcb* running_thread = NULL;
	// printf("uthread_current : queue length = %d\n", queue_length(threads));
	for (int i = 0; i < queue_length(threads); i++){
		// printf("uthread_current : about to declare t\n");
		void* t;
		// printf("uthread_current : about to peek\n");
		queue_peek(threads, &t);
		// printf("uthread_current : t initialized\n");
		struct uthread_tcb* checked_thread = (struct uthread_tcb*)t;
		// printf("uthread_current : checked_thread initialized\n");
		if (checked_thread->state == RUNNING){
			// printf("uthread_current : found running thread\n");
			running_thread = checked_thread;
		} else {
			// printf("uthread_current : moving element to back of queue\n");
			queue_move_front_to_back(threads);
			// printf("uthread_current : moved element to back of queue\n");
		}
	}

	// printf("uthread_current : exited loop\n");
	return running_thread;
}

struct uthread_tcb* find_thread_with_state(queue_t queue, enum states state){
	struct uthread_tcb* desired_thread = NULL;
	for (int i = 0; i < queue_length(threads); i++){
		void* t;
		queue_peek(threads, &t);
		struct uthread_tcb* checked_thread = (struct uthread_tcb*)t;
		if (desired_thread == NULL && checked_thread->state == state){
			desired_thread = checked_thread;
		} else {
			queue_move_front_to_back(threads);
		}
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
// 		} else {
// 			queue_move_front_to_back(threads);
// 		}
// 	}

// 	// printf("threads after uthread_yield: %d total\n", queue_length(threads));
// 	// queue_iterate(threads, print_tcb);

// 	struct uthread_tcb* next_thread = find_thread_with_state(threads, READY);
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

		// if it is the running thread, flag it as READY and use delete() and enqueue() to move it to the back
		if (checked_thread->state == RUNNING){
			running_thread = checked_thread;
			queue_delete(threads, checked_thread);
			running_thread->state = READY;
			queue_enqueue(threads, running_thread);
			break;
		} 
		
		// if we don't find the running thread, we move the front element to the back and continue checking
		else {
			queue_move_front_to_back(threads);
		}
	}

	// find the next READY thread, start it running, and switch contexts to it
	// this will resume the thread where it left off
	struct uthread_tcb* next_thread = find_thread_with_state(threads, READY);
	next_thread->state = RUNNING;
	uthread_ctx_switch(running_thread->context, next_thread->context);
}

void uthread_exit(void)
{
	/* TODO Phase 2 */
	struct uthread_tcb* exiting_thread;
	
	// we will look at every element of the queue, from front to back
	for (int i = 0; i < queue_length(threads); i++){

		// checked_thread is the thread at the front of the queue
		void* t;
		queue_peek(threads, &t);
		struct uthread_tcb* checked_thread = (struct uthread_tcb*)t;

		// if it is the running thread, we set it to be deleted
		if (checked_thread->state == RUNNING){
			queue_dequeue(threads, (void**)(&exiting_thread));
			break;
		} 
		
		// if we don't find the running thread, we move the front element to the back and continue checking
		else {
			queue_move_front_to_back(threads);
		}
	}

	// printf("\ndequeued zombie thread: now there are %d threads left\n", queue_length(threads));
	// printf("after dequeuing:\n");
	// queue_iterate(threads, print_tcb);
	bool should_switch_contexts = false;
	uthread_ctx_t* context_backup;

	if (queue_length(threads) > 0){
		struct uthread_tcb* next_thread = find_thread_with_state(threads, READY);

		if (next_thread == NULL){
			printf("error: deleted running thread and could not find replacement\n");
		}
		next_thread->state = RUNNING;
		should_switch_contexts = true;
		context_backup = next_thread->context;
	}

	uthread_ctx_destroy_stack(exiting_thread->stack_pointer);
	// printf("stack destroyed\n");
	free(exiting_thread->context);
	// printf("context freed\n");
	free(exiting_thread);
	// printf("thread freed\n");
	if (should_switch_contexts){
		// printf("about to switch contexts in uthread_exit\n");
		uthread_ctx_switch(exiting_thread->context, context_backup);
		// printf("switched contexts\n");
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

	// printf("\nthreads after uthread_create: %d total\n", queue_length(threads));
	// queue_iterate(threads, print_tcb);

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

	while (1){
		struct uthread_tcb* ready_thread = find_thread_with_state(threads, READY);
		if (ready_thread != NULL){
			// printf("found ready thread: %p\n", ready_thread->func);
			ready_thread->state = RUNNING;
			// printf("about to run thread func\n");
			ready_thread->func(ready_thread->args);
			// printf("function finished\n\n");
			// printf("%d\n", ready_thread->state);
			// ready_thread->state = ZOMBIE;
			// printf("zombified thread\n");
			uthread_exit();
		} else {
			// printf("no more threads are ready\n");
			break;
		}
	}

	// printf("queue_length = %d\n", queue_length(threads));

	// for (int i = 0; i < queue_length(threads); i++){
	// 	void* t;
	// 	printf("about to peek at queue\n");
	// 	queue_peek(threads, &t);
	// 	// printf("about to iniitalize checked thread\n");
	// 	struct uthread_tcb* checked_thread = (struct uthread_tcb*)t;
	// 	// printf("initialized checked thread\n");
	// 	if (checked_thread->state == ZOMBIE){
	// 		printf("found zombie thread\n");
	// 		uthread_exit();
	// 		// printf("zombie thread cleaned\n");
	// 		i--;
	// 	} else {
	// 		queue_move_front_to_back(threads);
	// 	}
	// }

	queue_destroy(threads);

	return 0;
}

void uthread_block(void)
{
	/* TODO Phase 3 */

	// printf("running uthread_block\n");
	struct uthread_tcb* running_thread;
	for (int i = 0; i < queue_length(threads); i++){
		void* t;
		queue_peek(threads, &t);
		struct uthread_tcb* checked_thread = (struct uthread_tcb*)t;
		if (checked_thread->state == RUNNING){
			running_thread = checked_thread;
			queue_delete(threads, checked_thread);

			running_thread->state = BLOCKED;
			queue_enqueue(threads, running_thread);
		} else {
			queue_move_front_to_back(threads);
		}
	}

	// printf("blocked running thread\n");

	struct uthread_tcb* next_thread = find_thread_with_state(threads, READY);
	next_thread->state = RUNNING;
	// printf("enabled next ready thread: about to switch contexts\n");
	uthread_ctx_switch(running_thread->context, next_thread->context);
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
	// printf("running uthread_unblock on thread with state %d\n", uthread->state);
	if (uthread->state == BLOCKED){
		uthread->state = READY;
	// printf("state switched from BLOCKED to READY\n");
	}
}