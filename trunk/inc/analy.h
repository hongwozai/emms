/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：analy.h
 ** 创建人：路泽亚
 ** 描  述：解析还原模块对主控模块的接口
 ** 注  意：1.
 **
 ** 当前版本： v2.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-18
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-11-30
 ********************************************************************/
#ifndef ANALY_H
#define ANALY_H

extern int  analy_init(char *device);
extern int  analy_run(void);
extern void analy_stop(void);
extern void analy_close(void);

#endif /* ANALY_H */
