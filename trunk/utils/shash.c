/********************************************************************
 ** Copyright(c) 2016,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：shash.c
 ** 创建人：路泽亚
 ** 描  述：垃圾邮件使用的哈希表
 ** 注  意：
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2016-01-17
 ********************************************************************/
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "mem.h"
#include "byte.h"
#include "shash.h"

#define T        struct Shash
#define NODE_T   struct Shash_node
#define BUCKET_T struct Shash_bucket

struct Shash_node {
    char *key;
    struct Shash_node *next;
};

struct Shash_bucket {
    struct Shash_node *list;
};

struct Shash {
    int len;
    int size;
    struct Shash_bucket *buckets;

    /* 遍历到的节点及索引，以便下一次继续遍历 */
    int cur_index;
    struct Shash_node *cur_node;
};

/* 字符串的哈希算法 */
static void free_node(NODE_T *node);
static void free_bucket(BUCKET_T *bucket);
static inline unsigned long str_hash(char *key);
static inline int str_equal(char *one, char *two);

T* Shash_new(int len, int size)
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
    hash->cur_index = 0;
    hash->cur_node  = NULL;
    return hash;
}

void Shash_free(T *hash)
{
    assert(hash);
    /* 注意还要释放桶内节点 */
    int i;
    for (i = 0; i < hash->len; ++i) {
        free_bucket(&hash->buckets[i]);
    }
    FREE(hash->buckets);
    FREE(hash);
}

void *Shash_find(T *hash, char *key)
{
    int index;
    NODE_T *p, *value = NULL;
    BUCKET_T *bucket;

    assert(key);
    assert(hash);
    index = MOD(str_hash(key), hash->len);
    bucket = &hash->buckets[index];

    p = bucket->list;
    while (p) {
        if (str_equal(key, p->key)) {
            value = p;
            return (void*)value + sizeof(NODE_T);
        }
        p = p->next;
    }
    return NULL;
}

int Shash_insert(T *hash, char *key, void *value)
{
    int index;
    NODE_T *real, *p;
    BUCKET_T *bucket;

    assert(key);
    assert(hash);
    if (!ALLOC(real, hash->size + sizeof(NODE_T))) {
        return -1;
    }
    if (!ALLOC(real->key, strlen(key) + 1)) {
        FREE(real);
        return -1;
    }
    strcpy(real->key, key);
    memcpy((char*)real + sizeof(NODE_T), value, hash->size);

    index = MOD(str_hash(key), hash->len);
    bucket = &hash->buckets[index];

    p = bucket->list;
    real->next  = p;
    bucket->list = real;
    return 0;
}

int Shash_del(T *hash, char *key)
{
    int index, flag = 0;
    NODE_T **p, *q;
    BUCKET_T *bucket;

    assert(key);
    assert(hash);
    index  = MOD(str_hash(key), hash->len);
    bucket = &hash->buckets[index];

    if (!bucket->list)
        return -1;

    p = &bucket->list;
    do {
        q = (*p)->next;
        if (str_equal((*p)->key, key)) {
            FREE(*p);
            *p   = q;
            flag = 1;
            break;
        }
        p = &(*p)->next;
    } while (q);
    return flag? 0 : -1;
}

/* 每次若想从头开始编译就调用此函数 */
void Shash_rewalk(T *hash)
{
    assert(hash);
    hash->cur_node = NULL;
    hash->cur_index = -1;
}

/* key, value都是传出参数 */
/* 获取当前node的下一个，是真，就直接返回
 * 是假，就获取下一个bucket->list不为空的节点
 */
int Shash_walk(T *hash, char **key, void **value)
{
    BUCKET_T *bucket;

    assert(key);
    assert(hash);
    assert(value);
    while (1) {
        if (!hash->cur_node) {
            do {
                ++hash->cur_index;
                if (hash->cur_index > hash->len - 1)
                    /* 遍历完成 */
                    return -1;
                bucket = &hash->buckets[hash->cur_index];
                hash->cur_node = bucket->list;
            } while (!hash->cur_node);
            *key = hash->cur_node->key;
            *value = (char*)hash->cur_node + sizeof(NODE_T);
            return 0;
        }
        hash->cur_node = hash->cur_node->next;
        if (hash->cur_node) {
            *key = hash->cur_node->key;
            *value = (char*)hash->cur_node + sizeof(NODE_T);
            return 0;
        }
    }
}

/* hash算法及比较函数 */
static inline unsigned long str_hash(char *key)
{
    char ch;
    register size_t hash = 0;

    while ((ch = *key++)) {
        hash = hash * 131 + ch;
    }
    return hash;
}

static inline int str_equal(char *one, char *two)
{
    assert(one);
    assert(two);
    return !strcmp(one, two);
}

static void free_bucket(BUCKET_T *bucket)
{
    NODE_T *tmp, *p;

    assert(bucket);
    p = tmp = bucket->list;
    while (tmp) {
        p = p->next;
        free_node(tmp);
        tmp = p;
    }
}

static void free_node(NODE_T *node)
{
    assert(node);
    FREE(node->key);
    FREE(node);
}
