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
bool is_preempting;

struct uthread_tcb {
	void* stack_pointer;
	uthread_ctx_t* context;
	enum states{RUNNING, READY, BLOCKED, ZOMBIE} state;
	uthread_func_t func;
	void* args;
};

// was used for debugging: does not compile with -Wextra
// void print_tcb(queue_t queue, void* data){
// 	struct uthread_tcb* tcb = (struct uthread_tcb*)data;
// 	printf("TCB func pointer: %p\n", tcb->func);

// 	switch (tcb->state){
// 		case 0:
// 			printf("TCB state: RUNNING\n");
// 			break;
		
// 		case 1:
// 			printf("TCB state: READY\n");
// 			break;

// 		case 2:
// 			printf("TCB state: BLOCKED\n");
// 			break;

// 		case 3:
// 			printf("TCB state: ZOMBIE\n");
// 			break;

// 		default:
// 			printf("TCB somehow has an impossible state\n");
// 			break;
// 	}
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
		
		// otherwise, move it to the back and check the next item
		else {
			queue_move_front_to_back(threads);
		}
	}

	return running_thread;
}

// more generalized version of above
struct uthread_tcb* find_thread_with_state(enum states state){
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

void uthread_yield(void)
{
	if (is_preempting){
		preempt_enable();
	}

	// uthread_yield finds the running thread and sets it to READY
	struct uthread_tcb* running_thread;
	
	// we will look at every element of the queue, from front to back
	for (int i = 0; i < queue_length(threads); i++){

		// checked_thread is the thread at the front of the queue
		void* t;
		queue_peek(threads, &t);
		struct uthread_tcb* checked_thread = (struct uthread_tcb*)t;

		// printf("before if\n");

		// if it is the running thread, flag it as READY and use delete() and enqueue() to move it to the back
		if (checked_thread->state == RUNNING){
			running_thread = checked_thread;
			queue_delete(threads, checked_thread);
			running_thread->state = READY;
			queue_enqueue(threads, running_thread);
			// printf("chaning running\n");
			break;
		} 
		
		// if we don't find the running thread, we move the front element to the back and continue checking
		else {
			queue_move_front_to_back(threads);
			// printf("else statement\n");
		}
	}

	// find the next READY thread, start it running, and switch contexts to it
	// this will resume the thread where it left off
	struct uthread_tcb* next_thread = find_thread_with_state(READY);
	next_thread->state = RUNNING;
	preempt_disable();
	uthread_ctx_switch(running_thread->context, next_thread->context);
}

void uthread_exit(void)
{
	if (is_preempting){
		preempt_enable();
	}

	// find the running thread and delete it from the queue
	struct uthread_tcb* exiting_thread = uthread_current();
	queue_delete(threads, exiting_thread);


	// we should only switch contexts if there is another ready thread to switch contexts to
	bool should_switch_contexts = false;
	uthread_ctx_t* context_backup;

	if (queue_length(threads) > 0){
		struct uthread_tcb* next_thread = find_thread_with_state(READY);


		if (next_thread == NULL){
			printf("error: deleted running thread and could not find replacement\n");
			return;
		}
		
		// get ready to run the next thread		
		next_thread->state = RUNNING;
		should_switch_contexts = true;
		context_backup = next_thread->context;
	}

	exiting_thread->state = ZOMBIE;

	preempt_disable();

	// switch contexts if necessary
	if (should_switch_contexts){
		uthread_ctx_switch(exiting_thread->context, context_backup);
	}
}

int uthread_create(uthread_func_t func, void *arg)
{
	if (is_preempting){
		preempt_enable();
	}
	struct uthread_tcb* new_thread = malloc(sizeof(struct uthread_tcb));
	new_thread->stack_pointer = uthread_ctx_alloc_stack();
	new_thread->context = malloc(sizeof(uthread_ctx_t));

	if (uthread_ctx_init(new_thread->context, new_thread->stack_pointer, func, arg) == -1){
		fprintf(stderr, "error creating context in uthread_create: context likely already exists\n");
		return -1;
	}
	
	new_thread->func = func;
	new_thread->args = arg;
	new_thread->state = READY;

	queue_enqueue(threads, new_thread);

	preempt_disable();

	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */

	preempt_start(preempt);
	is_preempting = preempt;

	threads = queue_create();

	// register incoming function as current thread
	if (uthread_create(func, arg)){
		return -1;
	}

	// continuously run ready threads until there are none left
	while (1){
		struct uthread_tcb* ready_thread = find_thread_with_state(READY);
		if (ready_thread != NULL){
			ready_thread->state = RUNNING;
			ready_thread->func(ready_thread->args);
			uthread_exit();
		} else {
			break;
		}
	}

	// delete every zombie process
	for (int i = 0; i < queue_length(threads); i++){
		void* t;
		queue_peek(threads, &t);
		struct uthread_tcb* checked_thread = (struct uthread_tcb*)t;

		if (checked_thread->state == ZOMBIE){
			uthread_ctx_destroy_stack(checked_thread->stack_pointer);
			free(checked_thread->context);
			free(checked_thread);
		}
	}

	queue_destroy(threads);

	preempt_stop();

	return 0;
}

void uthread_block(void)
{
	if (is_preempting){
		preempt_enable();
	}

	// block the running thread
	struct uthread_tcb* running_thread = find_thread_with_state(RUNNING);
	running_thread->state = BLOCKED;

	// find the next ready thread and start running it
	struct uthread_tcb* next_thread = find_thread_with_state(READY);
	next_thread->state = RUNNING;
	uthread_ctx_switch(running_thread->context, next_thread->context);

	preempt_disable();
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	if (is_preempting){
		preempt_enable();
	}

	// unblock the thread
	if (uthread != NULL && uthread->state == BLOCKED){
		uthread->state = READY;
	}

	preempt_disable();
}