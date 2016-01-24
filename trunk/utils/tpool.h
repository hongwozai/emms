/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：tpool.h
 ** 创建人：路泽亚
 ** 描  述：线程池
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-23
 ********************************************************************/
#ifndef TPOOL_H
#define TPOOL_H

#define T Tpool_T

typedef struct tpool Tpool_T;

extern T*   Tpool_new(int num);
extern void Tpool_free(T *pool);
extern int  Tpool_run(T *pool);

/**
 * @param pool
 * @param func
 * @return 返回-1，代表入队失败（内存分配失败）
 *         返回大于等于0的值，代表任务队列长度
 */
extern int Tpool_addwork(T *pool, void (*func)(void*), void *args);

#undef T
#endif /* TPOOL_H */
