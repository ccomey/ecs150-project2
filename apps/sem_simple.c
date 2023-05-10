/*
 * Semaphore simple test
 *
 * Test the synchronization of three threads, by having them print messages in
 * a certain order.
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "../libuthread/sem.h"
#include "../libuthread/uthread.h"

sem_t sem1;
sem_t sem2;
sem_t sem3;

static void thread3(void *arg)
{
	(void)arg;

	// printf("thread 3 is about to start waiting for thread 1 to unblock\n");
	sem_down(sem3);		/* Wait for thread1 */
	printf("thread3\n");
	sem_up(sem2);		/* Unblock thread2 */
	// printf("unblocked thread 2\n");
}

static void thread2(void *arg)
{
	(void)arg;

	// printf("thread 2 is about to start waiting for thread 3 to unblock\n");
	sem_down(sem2);		/* Wait for thread 3 */
	printf("thread2\n");
	sem_up(sem1);		/* Unblock thread1 */
	// printf("unblocked thread 1\n");
}

static void thread1(void *arg)
{
	(void)arg;

	// printf("about to create threads\n");
	uthread_create(thread2, NULL);
	uthread_create(thread3, NULL);
	// printf("created threads\n");

	sem_up(sem3);		/* Unblock thread 3 */
	// printf("unblocked thread 3\n");
	// printf("thread 1 is about to start waiting for thread 2 to unblock\n");
	sem_down(sem1); 	/* Wait for thread 2 */
	printf("thread1\n");
}

int main(void)
{
	// printf("about to create sems\n");
	sem1 = sem_create(0);
	sem2 = sem_create(0);
	sem3 = sem_create(0);
	// printf("created sems\n");

	uthread_run(false, thread1, NULL);

	// printf("about to destroy sems\n");
	sem_destroy(sem1);
	sem_destroy(sem2);
	sem_destroy(sem3);
	// printf("destroyed sems\n");

	return 0;
}
