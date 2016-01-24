/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：mcq.c
 ** 创建人：路泽亚
 ** 描  述：多任务下的循环队列，每个节点分配好内存（并非指针），以此来加快速度
 ** 注  意：1.该实现会向上圆整
 **        2.有一个节点无法使用
 **        3.使用了intel系列的汇编sfence，lfence
 **
 ** 当前版本： v2.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-16
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-11-24
 ********************************************************************/
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "mcq.h"
#include "mem.h"
#include "byte.h"

#define T struct Mcq

#define INDEX(q, index)                         \
    ((q)->buf + ((q)->size * (index)))

struct Mcq {
    char *buf;
    int size;
    int len;
    int head, tail;
};

T *Mcq_new(int size, int len)
{
    T *queue;

    assert(len);
    assert(size);
    if (IS_POW_OF_TWO(len)) {
        if ((unsigned)len > 0x80000000)
            return NULL;
        len = roundup_pow_of_two(len);
    }
    if (!NEW0(queue))
        return NULL;
    if (!ALLOC(queue->buf, size * len)) {
        FREE(queue);
        return NULL;
    }
    queue->len  = len;
    queue->size = size;
    return queue;
}

void Mcq_free(T *q)
{
    assert(q);
    FREE(q->buf);
    FREE(q);
}

/* 拷贝data开头，size长度的数据到当前的节点中 */
int Mcq_enq(T *q, void *data)
{
    int l;

    assert(q);
    assert(data);
    l = MOD(q->tail + 1, q->len);
    if (l == q->head)
        return -1;
    /* 多线程情况下必须先分配内存（放入数据）再移动指针 */
    /* lfence 读屏障 */
    __asm__("lfence":::"memory");
    memcpy(INDEX(q, l), data, q->size);

    __sync_synchronize();
    q->tail = l;
    return 0;
}

/* 需要将void强转为指定类型指针 */
int Mcq_deq(T *q, void *data)
{
    int l;

    assert(q);
    if (q->head == q->tail)
        return -1;

    l = MOD(q->head + 1, q->len);
    /* sfence 写屏障 */
    __asm__("sfence":::"memory");
    memcpy(data, INDEX(q, l), q->size);

    __sync_synchronize();
    q->head = l;
    return 0;
}

int Mcq_empty(T *q)
{
    assert(q);
    return (q->head == q->tail) ? 1 : 0;
}

int Mcq_full(T *q)
{
    assert(q);
    return (MOD(q->tail + 1, q->len) == q->head)? 1 : 0;
}
