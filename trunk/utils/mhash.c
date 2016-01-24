/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：mhash.c
 ** 创建人：路泽亚
 ** 描  述：哈希表
 **
 ** 当前版本： v2.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-17
 ** 注  意：1.散列表的节点改为内存池分配
 **        2.多任务改为单任务，因为查找速度慢于复制1500字节的时间（加锁更慢），
 **          抓包线程再查找hash没有任何意义
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-11-23
 ********************************************************************/
#include <nids.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

#include "byte.h"
#include "mhash.h"

#define T        struct Mhash
#define NODE_T   struct Mhash_node
#define BUCKET_T struct Mhash_bucket

struct Mhash_bucket {
    struct Mhash_node *list;
};

struct Mhash {
    int len;
    int size;
    Mpool_T *pool;
    struct Mhash_bucket *buckets;
};

static inline unsigned long tuple4_hash(struct tuple4 *one)
    __attribute__((always_inline));
static inline int tuple4_equal(struct tuple4 *key1, struct tuple4 *key2)
    __attribute__((always_inline));

T* Mhash_new(int len, int size, Mpool_T *pool)
{
    T *hash;

    assert(len > 0);
    assert(size > 0);
    if (IS_POW_OF_TWO(len)) {
        if ((unsigned)len > 0x80000000)
            return NULL;
        len = roundup_pow_of_two(len);
    }
    if (!NEW0(hash))
        return NULL;
    if (!ALLOC(hash->buckets, len * sizeof(BUCKET_T))) {
        FREE(hash);
        return NULL;
    }
    memset(hash->buckets, 0, len * sizeof(BUCKET_T));
    hash->len  = len;
    hash->size = size;
    hash->pool = pool;
    return hash;
}

void Mhash_free(T *hash)
{
    assert(hash);
    FREE(hash->buckets);
    FREE(hash);
}

void *Mhash_find(T *hash, struct tuple4 *key)
{
    int index;
    NODE_T *p, *value = NULL;
    BUCKET_T *bucket;

    assert(key);
    assert(hash);
    index = MOD(tuple4_hash(key), hash->len);
    bucket = &hash->buckets[index];

    p = bucket->list;
    while (p) {
        if (tuple4_equal(key, &p->key)) {
            value = p;
            break;
        }
        p = p->next;
    }
    return (void*)value;
}

void *Mhash_insert(T *hash, struct tuple4 *key)
{
    int index;
    NODE_T *value, *p;
    BUCKET_T *bucket;

    assert(key);
    assert(hash);
    value = Mpool_alloc(hash->pool);
    if (!value) {
        return NULL;
    }
    value->key = *key;
    index = MOD(tuple4_hash(key), hash->len);
    bucket = &hash->buckets[index];

    p = bucket->list;
    value->next  = p;
    bucket->list = value;
    return (void*)value;
}

int Mhash_del(T *hash, struct tuple4 *key)
{
    int index, flag = 0;
    NODE_T **p, *q;
    BUCKET_T *bucket;

    assert(key);
    assert(hash);
    index  = MOD(tuple4_hash(key), hash->len);
    bucket = &hash->buckets[index];

    if (!bucket->list)
        return -1;

    p = &bucket->list;
    do {
        q = (*p)->next;
        if (tuple4_equal(&(*p)->key, key)) {
            Mpool_free(hash->pool, *p);
            *p   = q;
            flag = 1;
            break;
        }
        p = &(*p)->next;
    } while (q);
    return flag? 0 : -1;
}

/* =================== hash 算法 比较函数 ========================= */
/**
 * 算法过程为：
 * 取源地址与目的地址的高16位与变化频繁的端口做异或运算，为1
 * 取源地址与目的地址的地16为与知名端口做异或运算，为2
 * 1与64做模运算，2与1024做模运算
 * 最后，1放入高6位，2放入低10位
 */
static inline unsigned long tuple4_hash(struct tuple4 *key)
{
    register unsigned short h6;
    register unsigned short l10;

    h6  = ((key->daddr >> 16) ^ (key->saddr >> 16));
    l10 = (((key->daddr & 0x00FF) ^ (key->saddr & 0x00FF)) ^ (key->source));
    h6  = MOD(h6,  64);
    l10 = MOD(l10, 1024);
    return (h6 << 10) | l10;
}

/* 目的端口都一致，所以只比较其余三个 */
static inline int tuple4_equal(struct tuple4 *key1, struct tuple4 *key2)
{
    /* 因为将三个协议混在一起，所以还是四个都比较 */
    if (key1->source == key2->source &&
        key1->daddr  == key2->daddr  &&
        key1->saddr  == key2->saddr &&
        key1->dest   == key2->dest) {
        return 1;
    }
    return 0;
}
