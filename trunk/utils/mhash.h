/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：mhash.h
 ** 创建人：路泽亚
 ** 描  述：哈希表
 ** 注  意：1.已经不是多任务
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-17
 ********************************************************************/
#ifndef MHASH_H
#define MHASH_H
#include <nids.h>

#include "mem.h"

#define T Mhash_T
#define HASH_HEAD                               \
    struct Mhash_node __head

struct Mhash_node {
    struct tuple4 key;
    struct Mhash_node *next;
};

typedef struct Mhash Mhash_T;

extern T*    Mhash_new(int len, int size, Mpool_T *pool);
extern void  Mhash_free(T *hash);
extern void* Mhash_find(T *hash, struct tuple4 *key);
extern void* Mhash_insert(T *hash, struct tuple4 *key);
extern int   Mhash_del(T *hash, struct tuple4 *key);

#undef T
#endif /* MHASH_H */
