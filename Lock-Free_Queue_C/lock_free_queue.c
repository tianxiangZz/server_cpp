#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <stdio.h>
#include "lock_free_queue.h"

#define DEBUG_SWITCH_ON     (1)

#define DEBUG_INFO(fmt, ...)    do {    \
	if (DEBUG_SWITCH_ON) {              \
		printf("[F:%s][L:%d] => " fmt "\r\n", __FUNCTION__, __LINE__, ## __VA_ARGS__);  \
	} } while (0);



int lockFreeQueueInit(lockFreeQueue *thiz)
{
    lockfreeQueueNode *node = NULL;

    if (NULL == thiz)
    {
        return false;
    }

    memset(thiz, 0x00, sizeof(lockFreeQueue));

    if (NULL == (node = calloc(1, sizeof(lockfreeQueueNode))))
    {
        return false;
    }

    atomic_store(&node->next, NULL);


    node->next = NULL;
    thiz->head = thiz->tail = node;
    return true;
}

void lockFreeQueueDeInit(lockFreeQueue *thiz)
{
    // free node
    return ;
}

int lockFreeQueuePush(lockFreeQueue *thiz, void *data)
{
    lockfreeQueueNode *node = NULL, *p = NULL, *old = NULL;

    if (NULL == thiz || NULL == data)
        return false;

    if (NULL == (node = calloc(1, sizeof(lockfreeQueueNode))))
    {
        return false;
    }

    node->data = data;
    node->next = NULL;

    do
    {
        p = thiz->tail;
    } while (!atomic_compare_exchange_strong(p->next, old, (*node)));

    DEBUG_INFO();
    atomic_compare_exchange_strong(thiz->tail, p, *node);

    return true;
}

void *lockFreeQueuePop(lockFreeQueue *thiz)
{
    lockfreeQueueNode *p = NULL;

    if (NULL == thiz)
        return NULL;


    do
    {
        p = thiz->head;
        if (NULL == p->next)
        {
            return NULL;
        }
    } while (!atomic_compare_exchange_strong(thiz->head, p, *(p->next)));

    return p->next->data;
}

