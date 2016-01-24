/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：analy_webmail.h
 ** 创建人：路泽亚
 ** 描  述：解析还原模块的webmail部分
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-18
 ********************************************************************/
#ifndef ANALY_WEBMAIL_H
#define ANALY_WEBMAIL_H

#include <nids.h>

extern int  analy_webmail_init(void);
extern void analy_webmail_putin(struct tcp_stream *ts);
extern void analy_webmail_run(void);
extern void analy_webmail_stop(void);

#endif /* ANALY_WEBMAIL_H */
