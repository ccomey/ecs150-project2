/*
 * Thread creation and yielding test
 *
 * Tests the creation of multiples threads and the fact that a parent thread
 * should get returned to before its child is executed. The way the printing,
 * thread creation and yielding is done, the program should output:
 *
 * thread1
 * thread2
 * thread3
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../libuthread/uthread.h"

void thread3(void *arg)
{
	// printf("thread2 func beginning\n");
	(void)arg;

	// printf("thread3 about to yield\n");
	uthread_yield();
	printf("thread3\n");
}

void thread2(void *arg)
{
	// printf("thread2 func beginning\n");
	(void)arg;

	// printf("thread2 func about to create thread3\n");
	uthread_create(thread3, NULL);
	uthread_yield();
	printf("thread2\n");
}

void thread1(void *arg)
{
	// printf("thread1 func beginning\n");
	(void)arg;

	// printf("thread1 func about to create thread2\n");
	uthread_create(thread2, NULL);
	uthread_yield();
	printf("thread1\n");
	uthread_yield();
}

int main(void)
{
	uthread_run(false, thread1, NULL);
	return 0;
}
