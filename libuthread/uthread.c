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
	printf("TCB state: %d\n", tcb->state);
}

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
	struct uthread_tcb* running_thread = NULL;
	// printf("uthread_current : queue length = %d\n", queue_length(threads));
	for (unsigned i = 0; i < queue_length(threads); i++){
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

struct uthread_tcb* find_ready_thread(queue_t queue){
	/* TODO Phase 2/3 */
	struct uthread_tcb* ready_thread = NULL;
	for (unsigned i = 0; i < queue_length(threads); i++){
		void* t;
		queue_peek(threads, &t);
		struct uthread_tcb* checked_thread = (struct uthread_tcb*)t;
		if (ready_thread == NULL && checked_thread->state == READY){
			ready_thread = checked_thread;
		} else {
			queue_move_front_to_back(threads);
		}
	}

	return ready_thread;
}

void uthread_yield(void)
{
	/* TODO Phase 2 */
	// printf("\nin uthread_yield\n");
	struct uthread_tcb* running_thread;
	for (unsigned i = 0; i < queue_length(threads); i++){
		void* t;
		queue_peek(threads, &t);
		// printf("uthread_yield : initialized t\n");
		struct uthread_tcb* checked_thread = (struct uthread_tcb*)t;
		// printf("uthread_yield : initialized checked_thread\n");
		if (checked_thread->state == RUNNING){
			// printf("\nuthread_yield : found running thread: %p\n", checked_thread->func);
			running_thread = checked_thread;
			// printf("uthread_yield : about to delete checked thread\n");
			queue_delete(threads, checked_thread);
			// printf("uthread_yield : deleted checked thread\n");

			// printf("uthread_yield : about to set state to ready\n");
			running_thread->state = READY;
			// printf("uthread_yield : set state to ready\n");
			queue_enqueue(threads, running_thread);
			break;
		} else {
			queue_move_front_to_back(threads);
		}
	}

	// printf("threads after uthread_yield: %d total\n", queue_length(threads));
	// queue_iterate(threads, print_tcb);

	struct uthread_tcb* next_thread = find_ready_thread(threads);
	next_thread->state = RUNNING;
	// printf("about to switch contexts\n");
	uthread_ctx_switch(running_thread->context, next_thread->context);
	// printf("switched contexts\n");
}

/*
 * uthread_exit - Exit from currently running thread
 *
 * This function is to be called from the currently active and running thread in
 * order to finish its execution.
 *
 * This function shall never return.
 */

void uthread_exit(void)
{
	/* TODO Phase 2 */
	struct uthread_tcb* exiting_thread;
	queue_dequeue(threads, (void**)&exiting_thread);
	// printf("dequeued zombie thread\n");

	if (queue_length(threads) > 1){
		// printf("queue length before context switch: %d\n", queue_length(threads));
		struct uthread_tcb* next_thread = find_ready_thread(threads);
		// printf("about to switch contexts to %p\n", next_thread);
		next_thread->state = RUNNING;
		uthread_ctx_switch(exiting_thread->context, next_thread->context);
		// printf("switched contexts\n");
	}

	uthread_ctx_destroy_stack(exiting_thread->stack_pointer);
	// printf("stack destroyed\n");
	free(exiting_thread->context);
	// printf("context freed\n");
	free(exiting_thread);
	// printf("thread freed\n");
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
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */

	threads = queue_create();
	// printf("uthread_run ran\n");

	// register incoming function as current thread
	uthread_create(func, arg);
	// printf("queue length after creating initial thread: %d\n", queue_length(threads));

	// uthread_ctx_bootstrap(func, arg);

	while (1){
		struct uthread_tcb* ready_thread = find_ready_thread(threads);
		if (ready_thread != NULL){
			// printf("found ready thread: %p\n", ready_thread->func);
			ready_thread->state = RUNNING;
			// printf("about to run thread func\n");
			ready_thread->func(ready_thread->args);
			// printf("function finished\n\n");
			// printf("%d\n", ready_thread->state);
			ready_thread->state = ZOMBIE;
			// printf("zombified thread\n");
		} else {
			// printf("no more threads are ready\n");
			break;
		}
	}

	for (unsigned i = 0; i < queue_length(threads); i++){
		void* t;
		// printf("about to peek at queue\n");
		queue_peek(threads, &t);
		// printf("about to iniitalize checked thread\n");
		struct uthread_tcb* checked_thread = (struct uthread_tcb*)t;
		// printf("initialized checked thread\n");
		if (checked_thread->state == ZOMBIE){
			// printf("found zombie thread\n");
			uthread_exit();
			// printf("zombie thread cleaned\n");
			i--;
		} else {
			queue_move_front_to_back(threads);
		}
	}

	queue_destroy(threads);
}

void uthread_block(void)
{
	/* TODO Phase 3 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
}

/*
context.c functions:
void uthread_ctx_switch(uthread_ctx_t *prev, uthread_ctx_t *next)
	switch from one context to another

void *uthread_ctx_alloc_stack(void)
	allocate a stack pointer for a thread

void uthread_ctx_destroy_stack(void *top_of_stack)
	deallocate a stack pointer for a thread

static void uthread_ctx_bootstrap(uthread_func_t func, void *arg)
	runs function and exits thread

int uthread_ctx_init(uthread_ctx_t *uctx, void *top_of_stack, uthread_func_t func, void *arg)
	creates a context
*/