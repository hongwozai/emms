/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：log.h
 ** 创建人：路泽亚
 ** 描  述：日志模块，记录监测封堵日志
 ** 注  意：1.日志总共处理三个队列，webmail的封堵日志队列
 **          noweb的封堵日志队列，一个监测及垃圾邮件队列
 **        2.日志模块中使用mail池，这是analy中分配的
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-22
 ********************************************************************/
#ifndef LOG_H
#define LOG_H

#include "utils/acq.h"
#include "inc/global.h"

/* 通过enum PROTO 以及 enum TYPE引用 */
extern Acq_T *log_queue[2][4];

extern int  log_init(void);
extern void log_run(void);
extern int  log_push(Acq_T *mcq, mail_t *mail, char *keyword,
                     unsigned short port);
extern void log_stop(void);
extern void log_close(void);

#endif /* LOG_H */
