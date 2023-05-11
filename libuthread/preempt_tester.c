#include <stdio.h>
#include <stdbool.h>
#include "private.h"
#include "uthread.h"

void thread2(void){
    printf("let me have a turn\n");
}

void thread1(void){
    uthread_create(thread2, NULL);

    while (1){
        printf("I'm going to hog all your resources!\n");
    }
}

int main(){
    uthread_run(true, thread1, NULL);
}