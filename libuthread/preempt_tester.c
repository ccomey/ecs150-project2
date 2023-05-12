#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "private.h"
#include "uthread.h"

// should run after thread1 finishes completely, if preemption wasn't enabled
// however, with preemption enabled, it should print periodically before yielding to thread1
void thread2(void* arg){
    int i;
    for(i=0; i<10; i++){
        printf("let me have a turn\n");
        printf("Go back to thread1\n");
        uthread_yield();
    }
}

// should run indefinitely without yielding without preemption
// with preemption, should be interrupted periodically
void thread1(void* arg){
    uthread_create(thread2, NULL);
    while(1){
        sleep(0);
    }
}

// Thread 3-4 shows disable
// Without disable Thread 3-4 should be the same as thread 1-2 
// As preempt_disable is turn on thread 4 is never ran
void thread4(void* arg){
    int i;
    for(i=0; i<10; i++){
        printf("let me have a turn\n");
        printf("Go back to thread3\n");
        uthread_yield();
    }
}

//Creates thread 4 but never yields to it. As preemption is disable.
void thread3(void* arg){
    uthread_create(thread4, NULL);
    preempt_disable();
    while(1){
        sleep(0);
    }
}

//thread 5-6 show enable
//thread 5-6 is similar to thread 3-4. Howver we enable preempt back on.
//Will print periodically before yielding to thread5
void thread6(void* arg){
    int i;
    for(i=0; i<10; i++){
        printf("let me have a turn\n");
        printf("Go back to thread5\n");
        uthread_yield();
    }    
    
}

// creates thread 6 and disables preemption.
//We enable it so that preemtion works again.
void thread5(void* arg){
    uthread_create(thread6, NULL);
    preempt_disable();
    preempt_enable();
    while(1){
        sleep(0);
    }
}

//the first run is for uthreadrun
//the second run is disable
//the third run is for enable
int main(){
    uthread_run(true, thread1, NULL);
    //uthread_run(true, thread3, NULL);
    //uthread_run(true, thread5, NULL);

}
