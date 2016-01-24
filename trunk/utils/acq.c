/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：acq.c
 ** 创建人：路泽亚
 ** 描  述：异步队列 async circle queue
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-26
 ********************************************************************/
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

#include "mcq.h"
#include "mem.h"
#include "byte.h"

#define T struct Acq

struct Acq {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    Mcq_T *queue;
};

T *Acq_new(int size, int len)
{
    T *q;
    int flag;

    assert(len > 0);
    assert(size > 0);
    if (!NEW0(q)) {
        return NULL;
    }
    q->queue = Mcq_new(size, len);
    if (!q->queue) {
        FREE(q);
        return NULL;
    }
    flag = pthread_mutex_init(&q->mutex, NULL);
    if (flag) {
        Mcq_free(q->queue);
        FREE(q);
        return NULL;
    }
    flag = pthread_cond_init(&q->cond, NULL);
    if (flag) {
        Mcq_free(q->queue);
        pthread_mutex_destroy(&q->mutex);
        FREE(q);
        return NULL;
    }
    return q;
}

void Acq_free(T *q)
{
    assert(q);

    /* ?是否要先解锁 */
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
    Mcq_free(q->queue);
    FREE(q);
}

int Acq_enq(T *q, void *data)
{
    int flag;

    pthread_mutex_lock(&q->mutex);
    flag = Mcq_enq(q->queue, data);
    pthread_mutex_unlock(&q->mutex);
    pthread_cond_signal(&q->cond);
    return flag;
}

void Acq_deq(T *q, void *data)
{
    assert(q);
    assert(data);
    pthread_mutex_lock(&q->mutex);
    while (Mcq_empty(q->queue)) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    Mcq_deq(q->queue, data);
    pthread_mutex_unlock(&q->mutex);
}

int Acq_empty(T *q)
{
    assert(q);
    return Mcq_empty(q->queue);
}

int Acq_full(T *q)
{
    assert(q);
    return Mcq_full(q->queue);
}
