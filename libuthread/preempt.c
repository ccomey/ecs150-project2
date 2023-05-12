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
#define HZ 100
struct sigaction action;
struct sigaction old_action;
struct itimerval timer;

void sighandler(int signum){

	//gets called when alarm rings
	if (signum == SIGVTALRM){
		uthread_yield();
	}
}

void preempt_disable(void)
{
	/* TODO Phase 4 */
	//Disable preempt by blocking through sigprocmask
	sigset_t blockSignal;
	sigemptyset(&blockSignal);
	sigaddset(&blockSignal, SIGVTALRM);
	sigprocmask(SIG_BLOCK, &blockSignal, NULL);
}

void preempt_enable(void)
{
	/* TODO Phase 4 */
	//Enable preempt by unblocking through sigprocmask
	sigset_t blockSignal;
	sigemptyset(&blockSignal);
	sigaddset(&blockSignal, SIGVTALRM);
	sigprocmask(SIG_UNBLOCK, &blockSignal, NULL);

}

void preempt_start(bool preempt)
{
	/* TODO Phase 4 */
	//checks if preempt is true or false in uthread_run
	if (!preempt){
		return;
	}
	
	//Sets sighandler to action,
    sigemptyset(&action.sa_mask);
	action.sa_handler = sighandler;
	action.sa_flags = 0;

	//Ensures that SIGVTALARM is not blocked
	sigset_t blockSignal;
	sigemptyset(&blockSignal);
	sigaddset(&blockSignal, SIGVTALRM);
	sigprocmask(SIG_UNBLOCK, &blockSignal, NULL);

	//runs sigaction through action, also check if sigaction works, if not exit
	if (sigaction(SIGVTALRM, &action, &old_action) == -1) {
        perror("sigaction error\n");
        exit(1);
    }

	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 1000000 / HZ;
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 1000000 / HZ;

	//check if Timer works, if not exit
	if (setitimer(ITIMER_VIRTUAL, &timer, NULL) < 0){
		perror("setitimer error");
		exit(1);
	}
}

//Restore previous signal action
//disables timmer
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

