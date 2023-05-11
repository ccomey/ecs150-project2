#include <stddef.h>
#include <stdlib.h>

#include <stdio.h> //TODO: remove this

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	/* TODO Phase 3 */
	size_t access_count;
	queue_t blocked_accessors;
};

sem_t sem_create(size_t count)
{
	// printf("running sem_create\n");
	/* TODO Phase 3 */
	sem_t s = malloc(sizeof(sem_t));

	s->access_count = count;

	s->blocked_accessors = queue_create();

	return s;
}

/*
 * sem_destroy - Deallocate a semaphore
 * @sem: Semaphore to deallocate
 *
 * Deallocate semaphore @sem.
 *
 * Return: -1 if @sem is NULL or if other threads are still being blocked on
 * @sem. 0 is @sem was successfully destroyed.
 */
int sem_destroy(sem_t sem)
{
	/* TODO Phase 3 */
	if (sem == NULL || queue_destroy(sem->blocked_accessors) == -1){
		return -1;
	}

	free(sem);
	return 0;
}

int sem_down(sem_t sem)
{
	/* TODO Phase 3 */
	// printf("running sem_down\n");

	if (sem == NULL){
		// printf("passed in semaphore was null\n");
		return -1;
	}

	// lock
	// spinlock_lock(&(sem->lock));

	// block self if access count == 0
	while (sem->access_count == 0){
		// printf("access count is 0\n");
		// block self
		
		queue_enqueue(sem->blocked_accessors, uthread_current());

		uthread_block();
	}

	sem->access_count--;

	// unlock
	// spinlock_unlock(&sem->lock);

	return 0;
}

int sem_up(sem_t sem)
{
	/* TODO Phase 3 */
	// printf("running sem_up\n");

	if (sem == NULL){
		// printf("passed in semaphore was null\n");
		return -1;
	}

	// lock
	// spinlock_lock(&sem->lock);
	// printf("locked\n");


	sem->access_count += 1;
	// printf("incremented access count to %ld\n", sem->access_count);

	// unblock first member of queue
	void* t;
	queue_dequeue(sem->blocked_accessors, &t);

	if (t != NULL){
		struct uthread_tcb* tcb = (struct uthread_tcb*)t;
		uthread_unblock(tcb);
	}

	// printf("ran uthread_unblock in sem_up\n");

	// unlock
	// spinlock_unlock(&sem->lock);
	// printf("unlocked\n");

	return 0;
}



