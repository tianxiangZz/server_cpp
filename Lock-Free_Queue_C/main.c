#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "lock_free_queue.h"

lockFreeQueue gctx;
#define TEST_NUM    (10)

int data[TEST_NUM] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};


void *Producer(void *arg)
{
    for (int i = 0; i < TEST_NUM; ++ i)
    {
        printf("Producer BEGIN %d\n", i);
        lockFreeQueuePush(&gctx, data + i);
        printf("Producer END %d\n", i);
    }
}


void *Consumer(void *arg)
{
    int *_data = NULL;
    while (NULL != (_data = lockFreeQueuePop(&gctx)))
    {
        printf("Consumer %d\n", *_data);
        _data = NULL;
    }
}


int main()
{
    lockFreeQueueInit(&gctx);

    pthread_t pid[2];
    pthread_create(&pid[0], NULL, Producer, NULL);
    // pthread_create(&pid[1], NULL, Consumer, NULL);


    pthread_join(pid[0], NULL);
    // pthread_join(pid[1], NULL);

    return 0;
}


// gcc lock_free_queue.c main.c -o main -std=gnu99  -latomic