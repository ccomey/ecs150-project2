#include <stdio.h>
#include <assert.h>

#include "../libuthread/queue.h"

void it_func(queue_t q, void* data){
    int* a = (int*)data;

    if (*a == 42){
        queue_delete(q, data);
    } else {
        *a += 1;
    }
}

void print_ints(queue_t q, void* data){
    int* a = (int*)data;

    printf("%d\n", *a);
}

int main(){
    queue_t test_queue = queue_create();
    printf("created queue\n");

    int num = 31;
    char c = 'r';
    float f = 1.4f;

    queue_enqueue(test_queue, &num);
    queue_enqueue(test_queue, &c);
    queue_enqueue(test_queue, &f);

    for (unsigned i = 0; i < 3; i++){
        void* element;
        queue_dequeue(test_queue, &element);
        // printf("%p\n", element);
    }

    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};

    for (unsigned i = 0; i < sizeof(data) / sizeof(data[0]); i++){
        printf("%d\n", i);
        queue_enqueue(test_queue, &data[i]);
    }

    queue_iterate(test_queue, it_func);
    assert(data[0] == 2);
    assert(queue_length(test_queue) == 9);

    printf("past asserts\n");
    printf("queue size before dequeueing: %d\n", queue_length(test_queue));

    void* element;
    while (!queue_dequeue(test_queue, &element)){}
    printf("queue size = %d\n", queue_length(test_queue));

    for (unsigned i = 0; i < sizeof(data) / sizeof(data[0]); i++){
        printf("%d\n", data[i]);
    }

    int arr[] = {3, 4, 5};
    int invalid_num = 7;

    queue_enqueue(test_queue, &arr[0]);
    queue_enqueue(test_queue, &arr[1]);
    queue_enqueue(test_queue, &arr[2]);

    printf("\nbefore moving to back:\n");

    queue_iterate(test_queue, print_ints);

    queue_move_to_back(test_queue);

    printf("\nafter moving to back:\n");

    queue_iterate(test_queue, print_ints);

    // printf("finished adding elements: size %d\n", queue_length(test_queue));

    queue_delete(test_queue, &arr[0]);
    // printf("removed element: size %d\n", queue_length(test_queue));
    queue_delete(test_queue, &arr[0]);
    // printf("attempted to remove element again: size %d\n", queue_length(test_queue));
    queue_delete(test_queue, &invalid_num);
    // printf("attempted to remove invalid element: size %d\n", queue_length(test_queue));

    queue_dequeue(test_queue, &element);
    // printf("dequeued: size %d\n", queue_length(test_queue));

    queue_dequeue(test_queue, &element);
    // printf("dequeued: size %d\n", queue_length(test_queue));

    // queue_dequeue(test_queue, &element);
    // printf("dequeued: size %d\n", queue_length(test_queue));
    

    queue_destroy(test_queue);
    printf("destroyed queue\n");
}