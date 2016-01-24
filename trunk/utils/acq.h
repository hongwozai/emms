/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：acq.h
 ** 创建人：路泽亚
 ** 描  述：异步队列
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-26
 ********************************************************************/
#ifndef ACQ_H
#define ACQ_H

#define T Acq_T

typedef struct Acq Acq_T;

extern T*    Acq_new(int size, int len);
extern void  Acq_free(T *q);
extern int   Acq_enq(T *q, void *data);
extern int   Acq_deq(T *q, void *data);
extern int   Acq_full(T *q);
extern int   Acq_empty(T *q);

#undef T
#endif /* ACQ_H */
