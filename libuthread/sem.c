#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

_Atomic int test_and_set(int* mem){
	int oldval = *mem;
	*mem = 1;
	return oldval;
}

void spinlock_lock(int* lock){
	while (test_and_set(lock) == 1);
}

void spinlock_unlock(int* lock){
	*lock = 0;
}

struct semaphore {
	/* TODO Phase 3 */
	size_t access_count;
	queue_t blocked_accessors;
	int lock;
};

sem_t sem_create(size_t count)
{
	/* TODO Phase 3 */
	sem_t s = malloc(sizeof(sem_t));
	s->access_count = count;
	s->blocked_accessors = queue_create();
	s->lock = 0;

	return 0;
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

	// lock
	spinlock_lock(&sem->lock);

	// block self if access count == 0
	while (sem->access_count == 0){
		// block self
	}

	sem->access_count--;

	// unlock
	spinlock_unlock(&sem->lock);

	return 0;
}

int sem_up(sem_t sem)
{
	/* TODO Phase 3 */

	// lock
	spinlock_lock(&sem->lock);


	sem->access_count++;

	// unblock first member of queue

	// unlock
	spinlock_unlock(&sem->lock);

	return 0;
}



