#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 1000000
struct sigaction old_action;
struct itimerval timer;


void sighandler(int signum){
	if (signum == SIGVTALRM){
		//printf("sig\n");
		uthread_yield();
	}
}

void preempt_disable(void)
{
	/* TODO Phase 4 */
	sigset_t blockSignal;
	sigemptyset(&blockSignal);
	sigaddset(&blockSignal, SIGVTALRM);
	pthread_sigmask(SIG_BLOCK, &blockSignal, NULL);
}

void preempt_enable(void)
{
	/* TODO Phase 4 */
	sigset_t blockSignal;
	sigemptyset(&blockSignal);
	sigaddset(&blockSignal, SIGVTALRM);
	pthread_sigmask(SIG_UNBLOCK, &blockSignal, NULL);

}

void preempt_start(bool preempt)
{
	/* TODO Phase 4 */
	// cited source: https://stackoverflow.com/questions/17167949/how-to-use-timer-in-c
	if (!preempt){
		return;
	}
	
	struct sigaction action;
	action.sa_handler = sighandler;

	sigaction(SIGVTALRM, &action, &old_action);
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 0;
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 1000000 / HZ;
	setitimer(ITIMER_VIRTUAL, &timer, NULL);	

	while (1){
		getitimer(ITIMER_VIRTUAL, &timer);
		//printf("%ld\n", timer.it_value.tv_usec);
	}
}

void preempt_stop(void)
{
	/* TODO Phase 4 */

    sigaction(SIGVTALRM, &old_action, NULL);

    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_VIRTUAL, &timer, NULL);
}

