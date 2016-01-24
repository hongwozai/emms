/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：analy_noweb.h
 ** 创建人：路泽亚
 ** 描  述：非webmail部分
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-27
 ********************************************************************/
#ifndef ANALY_NOWEB_H
#define ANALY_NOWEB_H

#include <nids.h>

extern int  analy_noweb_init(void);
extern void analy_noweb_putin(struct tcp_stream *ts);
extern void analy_noweb_run(void);
extern void analy_noweb_stop(void);

#endif /* ANALY_NOWEB_H */
