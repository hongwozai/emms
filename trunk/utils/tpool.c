/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：tpool.c
 ** 创建人：路泽亚
 ** 描  述：线程池
 ** 注  意：1.
 **
 ** 当前版本： v2.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-27
 ** 更改日志：更改整体结构，不再使用无锁方式，因为还没有太好的办法来使线程阻塞
 **         而且每个线程一个工作队列的方式会极大的延迟任务的处理时间
 **         动态线程池会有一定几率死锁，暂时无法解决，重新更改为静态线程池
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-23
 ********************************************************************/
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "mem.h"
#include "list.h"
#include "tpool.h"
#include "systemlog.h"

#define T        struct tpool
#define TASK_T   struct tpool_task

struct tpool_task {
    void (*func)(void *);
    void *args;
};

/**
 * 线程池结构体
 * @current_num 当前线程个数
 * @origin_num 起始线程个数
 * @isshutdown 是否关闭，是条件之一
 * @queue 异步队列，其中记录的是任务
 * @id 记录所有线程的id,使用向量的方式
 */
struct tpool {
    int origin_num;
    int current_num;
    int isshutdown;
    pthread_t *id;
    /* 以下为异步队列 */
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    List_T *queue;
};

static void *thread_callback(void *data);

/**
 * @num 初始线程的数量
 */
T* Tpool_new(int num)
{
    T *pool;
    int flag;

    assert(num > 0);
    if (!NEW0(pool))
        return NULL;
    pool->queue = List_new(sizeof(TASK_T));
    if (!pool->queue) {
        FREE(pool);
        return NULL;
    }
    if (!CALLOC(pool->id, sizeof(pthread_t), num)) {
        List_free(pool->queue);
        FREE(pool);
        return NULL;
    }
    if ((flag = pthread_mutex_init(&pool->mutex, NULL))) {
        ERROR("mutex can't create!(%s)", strerror(flag));
        List_free(pool->queue);
        FREE(pool->id);
        FREE(pool);
        return NULL;
    }
    if ((flag = pthread_cond_init(&pool->cond, NULL))) {
        ERROR("cond can't create!(%s)", strerror(flag));
        List_free(pool->queue);
        FREE(pool->id);
        FREE(pool);
        return NULL;
    }
    pool->isshutdown  = 0;
    pool->current_num = 0;
    pool->origin_num  = num;
    return pool;
}

void Tpool_free(T *pool)
{
    int i;

    assert(pool);
    /* 原子操作不必加锁，尤其是被限制到了一个读一个写的情况 */
    pool->isshutdown = 1;
    pthread_cond_broadcast(&pool->cond);

    /* TODO: 应设置一个变量来控制是等待还是直接干掉 */
    for (i = 0; i < pool->current_num; i++) {
        /* pthread_cancel(pool->id[i]); */
        pthread_join(pool->id[i], NULL);
    }
    FREE(pool->id);
    /* 后释放空间 */
    List_free(pool->queue);
    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cond);
    FREE(pool);
}

int Tpool_run(T *pool)
{
    int i, flag;

    assert(pool);
    for (i = 0; i < pool->origin_num; i++) {
        if ((flag = pthread_create(&pool->id[i], NULL,
                                   thread_callback, pool))) {
            ERROR("tpool create thread error!(%s)", strerror(flag));
            return -1;
        }
        pool->current_num++;
    }
    return 0;
}

int Tpool_addwork(T *pool, void (*func)(void*), void *args)
{
    int ret = -1;
    TASK_T task;

    assert(pool);
    assert(func);

    task.func = func;
    task.args = args;

    pthread_mutex_lock(&pool->mutex);
    if (pool->isshutdown) {
        pthread_mutex_unlock(&pool->mutex);
        pthread_cond_broadcast(&pool->cond);
        return ret;
    }
    ret = List_push(pool->queue, &task);
    pthread_mutex_unlock(&pool->mutex);
    pthread_cond_signal(&pool->cond);
    return ret;
}

static void *thread_callback(void *data)
{
    int ret = -1;
    T *pool = (T*)data;
    TASK_T task;

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    for (;;) {
        pthread_mutex_lock(&pool->mutex);
        while (!pool->isshutdown && List_empty(pool->queue) ) {
            pthread_cond_wait(&pool->cond, &pool->mutex);
        }
        if (pool->isshutdown) {
            pthread_mutex_unlock(&pool->mutex);
            pthread_cond_broadcast(&pool->cond);
            break;
        }
        ret = List_popleft(pool->queue, &task);
        if (ret == -1) {
            pthread_mutex_unlock(&pool->mutex);
            continue;
        }
        pthread_mutex_unlock(&pool->mutex);
        task.func(task.args);
    }
    pthread_exit(NULL);
}
