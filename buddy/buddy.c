

/********************************* INCLUDE ****************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "buddy.h"

/********************************* LOGGER **********************************************/

#define DEBUG_SWITCH_ON     (1)

#define DEBUG_INFO(fmt, ...)    do {    \
	if (DEBUG_SWITCH_ON) {              \
		printf("[F:%s][L:%d] => " fmt "\r\n", __FUNCTION__, __LINE__, ## __VA_ARGS__);  \
	} } while (0);

/********************************* DEFINE **********************************************/

#define BUDDY_LOCK(lock)            pthread_spin_lock(lock)
#define BUDDY_UNLOCK(lock)          pthread_spin_unlock(lock)

/********************************* TOOL **********************************************/

/**
 * 二叉树节点关系  下划线开头为  位运算优化
 */

#define _LEFT_LEAF(index)    	((index) << 1 | 1)              /** 二叉树 左叶子 */
#define LEFT_LEAF(index)    	((index) * 2 + 1)               /** 二叉树 左叶子 */

#define _RIGHT_LEAF(index)   	(((index) + 1) << 1)            /** 二叉树 右叶子 */
#define RIGHT_LEAF(index)   	((index) * 2 + 2)               /** 二叉树 右叶子 */

#define _PARENT(index)       	(((index) - 1) >> 1)         	/** 二叉树 父亲 */
#define PARENT(index)       	(((index) + 1) / 2 - 1)         /** 二叉树 父亲 */

#define IS_POWER_OF_2(x)    (!((x) & ((x) - 1)))            /** 判断是否是2的幂次 */
#define MAX(a, b)           ((a) > (b) ? (a) : (b))

/**
 * LOG2 可以求X为2的几次幂
 * 使用uint8存储2的幂次        在64位操作系统,理论上的寻址范围是2的64次幂
 * 因此 以65作为 特殊标记， 标记改位置已经被使用
 */
#define LOG2(x)             (8 * sizeof(size_t) - __builtin_clzl((size_t)x) - 1)
#define LOG2_0()			(8 * sizeof(size_t) + 1)
#define POW2(x)             ({x != LOG2_0() ? ((size_t)1 << (x)) : 0;}) 




/********************************* TYPE **********************************************/




/********************************* PRIVATE FUNC **********************************************/

// 将size 提升至 2的整数次幂
static inline size_t fixSize(size_t size)
{
	size -= 1;
	size |= size >> 1;
	size |= size >> 2;
	size |= size >> 4;
	size |= size >> 8;
	size |= size >> 16;
#if __SIZEOF_POINTER__ == 8
	size |= size >> 32;
#endif
	return size + 1;
}

static size_t _buddy_alloc(buddy *thiz, size_t size)
{
	size_t node_size = 0, index = 0, offset = 0;

	if (NULL == thiz || size < 1)
	{
		DEBUG_INFO(); return -1;
	}

	if (!IS_POWER_OF_2(size))
	{
		size = fixSize(size);
		DEBUG_INFO("fixsize = %ld", size);
	}

	BUDDY_LOCK(&(thiz->locK));

	// 从跟节点查找size大小
	if (POW2(thiz->longest[0]) < size)
	{
		BUDDY_UNLOCK(&(thiz->locK));
		DEBUG_INFO("size too big"); return -1;
	}

	for (node_size = thiz->size; node_size != size; node_size >>= 1)
	{
		if (POW2(thiz->longest[LEFT_LEAF(index)]) >= size)
		{
			index = LEFT_LEAF(index);
		}
		else
		{
			index = RIGHT_LEAF(index);
		}
	}

	thiz->used += POW2(thiz->longest[index]);
	thiz->longest[index] = LOG2_0();
	offset = (index + 1) * node_size - thiz->size;  /// TODO...

	// 更新父节点可用内存信息
	while (index)
	{
		index = PARENT(index);

		if (thiz->longest[LEFT_LEAF(index)] == LOG2_0())
		{
			thiz->longest[index] = thiz->longest[RIGHT_LEAF(index)];
		}
		else if (thiz->longest[RIGHT_LEAF(index)] == LOG2_0())
		{
			thiz->longest[index] = thiz->longest[LEFT_LEAF(index)];
		}
		else
		{
			thiz->longest[index] = MAX(thiz->longest[LEFT_LEAF(index)], 
				thiz->longest[RIGHT_LEAF(index)]);
		}
	}

	BUDDY_UNLOCK(&(thiz->locK));

	return offset;
}

static void _buddy_free(buddy *thiz, size_t offset)
{
	size_t node_size = 1, index = offset + thiz->size - 1;	/// TODO...
	size_t left_longest = 0, right_longest = 0;
	
	if (NULL == thiz || offset > thiz->size)
	{
		DEBUG_INFO(); return ;
	}

	BUDDY_LOCK(&(thiz->locK));

	// 从下往上  找到分配offset的节点
	for (; thiz->longest[index] != LOG2_0(); index = PARENT(index))
	{
		node_size <<= 1;
		if (index == 0)		// 未找到
		{
			BUDDY_UNLOCK(&(thiz->locK));
			return ;
		}
	}

	// 恢复节点可用内存
	thiz->longest[index] = LOG2(node_size);
	thiz->used -= POW2(thiz->longest[index]);

	// 更新父节点  合并子块
	while (index)
	{
		index = PARENT(index);
		node_size <<= 1;

		left_longest = POW2(thiz->longest[LEFT_LEAF(index)]);
		right_longest = POW2(thiz->longest[RIGHT_LEAF(index)]);

		if (left_longest + right_longest == node_size)
		{
			thiz->longest[index] = LOG2(node_size);
		}
		else
		{
			thiz->longest[index] = LOG2(MAX(left_longest, right_longest));
		}
	}

	BUDDY_UNLOCK(&(thiz->locK));
	return ;
}


/********************************* PUBLIC FUNC **********************************************/


int buddy_init(buddy *thiz, size_t size)
{
	int i = 0;
	size_t node_size = 0;

	if (NULL == thiz || size < 1 || !IS_POWER_OF_2(size))
	{
		DEBUG_INFO(); return BUDDY_ERROR;
	}

	memset(thiz, 0x00, sizeof(buddy));

	thiz->size = size;
	node_size = size * 2;

	for (int i = 0; i < (2 * size - 1); ++ i)
	{
		if (IS_POWER_OF_2(i + 1))
			node_size /= 2;

		thiz->longest[i] = LOG2(node_size);
		// DEBUG_INFO("[%d]:node_size = %lu, longest = %u", i, node_size, thiz->longest[i]);
	}

	if (0 != pthread_spin_init(&(thiz->locK), PTHREAD_PROCESS_PRIVATE))
	{
		DEBUG_INFO("lock init error!"); return BUDDY_ERROR;
	}

	return BUDDY_OK;
}

void buddy_Deinit(buddy *thiz)
{
	pthread_spin_destroy(&(thiz->locK));
	return ;
}

void *buddy_alloc(buddy *thiz, size_t size)
{
	if (NULL == thiz || size < 1)
	{
		DEBUG_INFO(); return NULL;
	}

	size_t offset = _buddy_alloc(thiz, size);
	if (offset == (size_t)-1)
	{
		DEBUG_INFO(); return NULL;
	}

	return thiz->data + (offset * BUDDY_MEMORY_BLOCK_SIZE);
}

void *buddy_calloc(buddy *thiz, size_t size)
{
	void *ptr = NULL;
	if (NULL != (ptr = buddy_alloc(thiz, size)))
	{
		DEBUG_INFO(); return NULL;
	}
	memset(ptr, 0x00, size);
	return ptr;
}

void buddy_free(buddy *thiz, void *ptr)
{
	if (NULL == thiz || (ptr - thiz->data) > thiz->size)
	{
		return ;
	}
	_buddy_free(thiz, (ptr - thiz->data));
	return ;
}

void buddy_show(buddy *thiz)
{
	if (NULL == thiz)
		return ;
	
	printf("Total size = %ld, Used = %ld, Base %p\r\n", 
		thiz->size, thiz->used, thiz->data);

	for (int i = 0; i < 2 * (thiz->size) - 1; ++ i)	
	{
		DEBUG_INFO("[%d]: longest = %u, node_size = %ld", 
			i, thiz->longest[i], POW2(thiz->longest[i]));
	}
	return ;
}
