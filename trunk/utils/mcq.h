/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：mcq.h
 ** 创建人：路泽亚
 ** 描  述：多任务下的循环队列，每个节点事先分配内存（动态数组）
 ** 注  意：1.
 **
 ** 当前版本： v2.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-16
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-11-24
 ********************************************************************/
#ifndef MCQ_H
#define MCQ_H

#define T Mcq_T

typedef struct Mcq Mcq_T;

extern T*    Mcq_new(int size, int len);
extern void  Mcq_free(T *q);
extern int   Mcq_enq(T *q, void *data);
extern int   Mcq_deq(T *q, void *data);
extern int   Mcq_full(T *q);
extern int   Mcq_empty(T *q);

#undef T
#endif /* MCQ_H */
