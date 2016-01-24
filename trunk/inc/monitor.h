/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：monitor.h
 ** 创建人：路泽亚
 ** 描  述：监测模块，从两个协议的监测队列中取出，并放入对应的日志队列中
 ** 注  意：1.这个模块存在是为了提高分析模块的速度
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-22
 ********************************************************************/
#ifndef MONITOR_H
#define MONITOR_H

#include "utils/acq.h"
#include "inc/global.h"

/* enum PROTO */
extern Acq_T *monitor_queue[2];

extern int  monitor_init(void);
extern int  monitor_push(Acq_T *mcq, mail_t *mail, unsigned short port);
extern void monitor_run(void);
extern void monitor_stop(void);
extern void monitor_close(void);

#endif /* MONITOR_H */
