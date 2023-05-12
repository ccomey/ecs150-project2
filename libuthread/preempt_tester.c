#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "private.h"
#include "uthread.h"

void thread2(void* arg){
    int i;
    for(i=0; i<10; i++){
        printf("let me have a turn\n");
        printf("Go back to thread1\n");
        uthread_yield();
    }
}

void thread1(void* arg){
    uthread_create(thread2, NULL);
    while(1){
        sleep(0);
    }
}

int main(){
    uthread_run(true, thread1, NULL);
}
