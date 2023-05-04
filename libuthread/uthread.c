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

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
}

void uthread_yield(void)
{
	/* TODO Phase 2 */
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
	uthread_ctx_destroy_stack(exiting_thread->stack_pointer);

	struct uthread_tcb* next_thread;
	queue_peek(threads, (void**)&next_thread);
	uthread_ctx_switch(exiting_thread->context, next_thread->context);
	free(exiting_thread);
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
	struct uthread_tcb* new_thread = malloc(sizeof(struct uthread_tcb));
	new_thread->stack_pointer = uthread_ctx_alloc_stack();

	if (uthread_ctx_init(new_thread->context, new_thread->stack_pointer, func, arg) == -1){
		fprintf(stderr, "error creating context in uthread_create: context likely already exists\n");
	}
	
	new_thread->func = func;
	new_thread->args = arg;
	new_thread->state = READY;

	queue_enqueue(threads, new_thread);
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */

	threads = queue_create();

	// register current execution flow as idle thread
	// uthread_create();


	// register incoming function as current thread
	uthread_create(func, arg);

	// uthread_ctx_bootstrap(func, arg);
}

void uthread_block(void)
{
	/* TODO Phase 3 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
}

