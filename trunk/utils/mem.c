/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：mem.c
 ** 创建人：路泽亚
 ** 描  述：内存池的实现
 ** 注  意：1.内存池分为数个内存块，每个内存块中有数个内存节点
 **        2.该内存池使用基于CAS的无锁链式堆栈算法
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-16
 ********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "mem.h"
#include "byte.h"

#define T       struct Mpool
#define NODE_T  struct Mpool_node
#define BLOCK_T struct Mpool_block

#define BLOCK_LEN(size, step)                               \
    (((size + sizeof(NODE_T)) * step) + sizeof(BLOCK_T))

#define PTR_ADD(p, d) ((void*)(p) + (d))
#define PTR_SUB(p, d) ((void*)(p) - (d))

#define CAS(ptr, oldval, newval)                        \
    __sync_bool_compare_and_swap(ptr, oldval, newval)

struct Mpool_node {
    struct Mpool_node *next;
};

struct Mpool_block {
    struct Mpool_block *next;
};

/**
 * size 内存节点的大小
 * step 步长，每次分配的内存块的内存节点数量
 * list 内存块链表，暂时只用来释放
 * free 空闲内存节点的链表，用来分配
 */
struct Mpool {
    int size;
    int step;
    int count;
    struct Mpool_block *list;
    struct Mpool_node  *free;
};

T* Mpool_new(int size, int step)
{
    T *pool;
    int amount, i;
    NODE_T *tmp, *p = NULL;

    assert(size);
    assert(step);
    if (!NEW0(pool))
        return NULL;

    if (!IS_MULTIPLE_OF_ADDR(size))
        size   = ROUND_UP_OF_ADDR(size);
    amount = BLOCK_LEN(size, step);
    if (!ALLOC(pool->list, amount)) {
        FREE(pool);
        return NULL;
    }
    pool->size  = size;
    pool->step  = step;
    pool->count = 1;

    /* 块链表 */
    pool->list->next = NULL;
    /* free链表 */
    pool->free = PTR_ADD(pool->list, sizeof(BLOCK_T));
    tmp = pool->free;
    for (i = 0; i < step; i++) {
        p = tmp;
        tmp = PTR_ADD(tmp, size + sizeof(NODE_T));
        p->next = tmp;
    }
    p->next = NULL;
    return pool;
}

void Mpool_dispose(T *pool)
{
    assert(pool);
    while (pool->list) {
        BLOCK_T *tmp;
        tmp = pool->list;
        pool->list = pool->list->next;
        FREE(tmp);
    }
    FREE(pool);
}

/* 第一种操作，正常，串行 */
/* tmp = PTR_ADD(pool->free, sizeof(NODE_T)); */
/* pool->free = pool->free->next; */
/* return tmp; */
void* Mpool_alloc(T *pool)
{
    int i;
    NODE_T *tmp, *head, *p = NULL;
    BLOCK_T *btmp;

    assert(pool);
    if (!pool->free) {
        /* 申请块,再分配 */
        if (!ALLOC(btmp, BLOCK_LEN(pool->size, pool->step))) {
            FREE(pool);
            return NULL;
        }
        pool->count++;
        /* 块链表 */
        btmp->next = pool->list;
        pool->list = btmp;
        /* free链表 */
        tmp = head = PTR_ADD(pool->list, sizeof(BLOCK_T));
        for (i = 0; i < pool->step; i++) {
            p = tmp;
            tmp = PTR_ADD(tmp, pool->size + sizeof(NODE_T));
            p->next = tmp;
        }
        p->next = NULL;
        /* FIXME: 这里有个问题，并行情况下，
         * 如果申请块时有内存释放那么这里就会造成内存池中的内存泄漏 */
        /* 想法，可以判断pool->free是否为空了，为空则直接赋值，不为空就链到后面,
         * 要想这样就需要一个刚分配块链表的尾指针 */
        pool->free = head;
    }

    /* 分配 */
    /* 第二种，无锁，弹栈要判断是否为空，判断tmp为真的情况是因为若是栈空也会跳出循环，
     * 不过因为EMMS项目该池只是单生产者单消费者，所以不可能有两个同时弹栈的线程 */
    for (;;) {
        tmp = pool->free;
        if (tmp && CAS(&pool->free, tmp, tmp->next)) {
            break;
        }
    }
    return PTR_ADD(tmp, sizeof(NODE_T));
}

/* 第一种操作,正常，串行 */
/* tmp->next = pool->free; */
/* pool->free = tmp; */
void Mpool_free(T *pool, void *data)
{
    NODE_T *tmp;

    assert(pool);
    assert(data);
    tmp = PTR_SUB(data, sizeof(NODE_T));

    /* 第二种, 无锁 */
    for (;;) {
        tmp->next = pool->free;
        if (CAS(&pool->free, tmp->next, tmp)) {
            break;
        }
    }
}
