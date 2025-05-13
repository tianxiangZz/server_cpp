#ifndef __BUDDY_H__
#define __BUDDY_H__

/********************************* INCLUDE ****************************************** */
#define __USE_XOPEN2K   1
#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>

/********************************* DEFINE **********************************************/

#define BUDDY_OK        (0)
#define BUDDY_ERROR     (-1)

// 修改这个即可
#define BUDDY_MEMORY_SIZE               (8)
#define BUDDY_MEMORY_BLOCK_SIZE         (1)


/********************************* TYPE **********************************************/


/**
 * size 实际上表示的是 块， 即有多少个块参与分配
 * size = BUDDY_MEMORY_SIZE / BUDDY_MEMORY_BLOCK_SIZE， 且必须是2的整数次幂
 * 
 * longest: 完全二叉树表示，块的使用状态
 * longest = size * 2 - 1个 = 完全二叉树的节点个数
 * 
 */


typedef struct _buddy
{
    atomic_int init_flag;
    pthread_spinlock_t locK;

    size_t size;
    size_t used;
    uint8_t longest[(BUDDY_MEMORY_SIZE / BUDDY_MEMORY_BLOCK_SIZE) * 2];

    void *data;
} buddy;


/********************************* PUBLIC FUNC **********************************************/

// size = BUDDY_MEMORY_SIZE / BUDDY_MEMORY_BLOCK_SIZE
int buddy_init(buddy *thiz, size_t size);
void buddy_Deinit(buddy *thiz);

void *buddy_alloc(buddy *thiz, size_t size);
void *buddy_calloc(buddy *thiz, size_t size);
void buddy_free(buddy *thiz, void *ptr);


void buddy_show(buddy *thiz);

#endif /** __BUDDY_H__  */