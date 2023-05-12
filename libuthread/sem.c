#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

// simply contains access count and queue of blocked threads trying to access it
struct semaphore {
	/* TODO Phase 3 */
	size_t access_count;
	queue_t blocked_accessors;
};

// dynamically allocate a new semaphore and set up its members
sem_t sem_create(size_t count)
{
	/* TODO Phase 3 */
	sem_t s = malloc(sizeof(sem_t));

	s->access_count = count;

	s->blocked_accessors = queue_create();

	return s;
}

// delete the semaphore, but only if its queue is empty
int sem_destroy(sem_t sem)
{
	/* TODO Phase 3 */
	if (sem == NULL || queue_destroy(sem->blocked_accessors) == -1){
		return -1;
	}

	free(sem);
	return 0;
}

// free a resource, and unblock a thread if it is the only free resource
int sem_down(sem_t sem)
{
	/* TODO Phase 3 */

	if (sem == NULL){
		return -1;
	}

	while (sem->access_count == 0){
		// block self
		queue_enqueue(sem->blocked_accessors, uthread_current());
		uthread_block();
	}

	sem->access_count--;

	return 0;
}

// block a resource, and block the running thread if it was the last resource
int sem_up(sem_t sem)
{
	/* TODO Phase 3 */

	if (sem == NULL){
		return -1;
	}

	sem->access_count += 1;

	// unblock first member of queue
	void* t;
	queue_dequeue(sem->blocked_accessors, &t);

	if (t != NULL){
		struct uthread_tcb* tcb = (struct uthread_tcb*)t;
		uthread_unblock(tcb);
	}

	return 0;
}



