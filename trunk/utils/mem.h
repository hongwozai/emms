/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：mem.h
 ** 创建人：路泽亚
 ** 描  述：对内存管理的封装，内存池等
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-16
 ********************************************************************/
#ifndef MEM_H
#define MEM_H

#include <stdlib.h>

/* 系统内存管理 */
#define NEW(p)                                  \
    ((p) = malloc(sizeof *(p)))

#define NEW0(p)                                 \
    ((p) = calloc(1, sizeof *(p)))

#define ALLOC(p, nbytes)                        \
    ((p) = malloc((nbytes)))

#define CALLOC(p, count, nbytes)                \
    ((p) = calloc((count), (nbytes)))

#define REALLOC(p, oldptr, nbytes)              \
    ((p) = realloc((oldptr), (nbytes)))

#define FREE(p)                                 \
    (free((p)))

/* 内存池 */
#define T Mpool_T

typedef struct Mpool Mpool_T;

T*    Mpool_new(int size, int step);
void  Mpool_dispose(T *pool);
void* Mpool_alloc(T *pool);
void  Mpool_free(T *pool, void *data);

#undef T
#endif /* MEM_H */
