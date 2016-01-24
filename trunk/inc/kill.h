/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：kill.h
 ** 创建人：路泽亚
 ** 描  述：封堵模块
 ** 注  意：封堵模块仅一个函数，会被解析模块调用，
 **        也不会创建封堵线程
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-11-27
 ********************************************************************/
#ifndef KILL_H
#define KILL_H

#include <nids.h>

extern int  kill_init();
extern void kill_tcp_with_protocol(struct tcp_stream *ts, int proto);
extern void kill_tcp_request(struct tcp_stream *ts);
extern void kill_tcp_response(struct tcp_stream *ts);
extern void kill_close();

#endif /* KILL_H */
