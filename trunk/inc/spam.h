/********************************************************************
 ** Copyright(c) 2016,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：spam.h
 ** 创建人：路泽亚
 ** 描  述：垃圾邮件模块
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2016-01-17
 ********************************************************************/
#ifndef SPAM_H
#define SPAM_H

extern int spam_init(double thresold, char *filename);
extern int  spam_train(const char *dir);
extern int  spam_filter(char *content);
extern void spam_close();

#endif /* SPAM_H */
