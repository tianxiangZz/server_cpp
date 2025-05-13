#ifndef __LOCK_FREE_QUEUE_H__
#define __LOCK_FREE_QUEUE_H__

typedef struct _lockfreeQueueNode lockfreeQueueNode;

struct _lockfreeQueueNode
{
    void *data;
    atomic_uintptr_t *next;
};

typedef struct _lockfreeQueue
{
    atomic_uintptr_t *head;
    atomic_uintptr_t *tail;
} lockFreeQueue;


int lockFreeQueueInit(lockFreeQueue *thiz);
void lockFreeQueueDeInit(lockFreeQueue *thiz);

int lockFreeQueuePush(lockFreeQueue *thiz, void *data);
void *lockFreeQueuePop(lockFreeQueue *thiz);



#endif /** __LOCK_FREE_QUEUE_H__ */