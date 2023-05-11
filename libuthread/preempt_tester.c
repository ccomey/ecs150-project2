#include <stdio.h>
#include <stdbool.h>
#include "private.h"
#include "uthread.h"

int main(){
    while (1){
        preempt_start(true);
    }
}