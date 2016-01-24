/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：db.h
 ** 创建人：王新
 ** 描  述：数据库部分
 ** 注  意：1.
 **
 ** 当前版本： v2.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-21
 **
 ** 当前版本： v1.0
 ** 作    者：王新
 ** 完成日期： 2015-12-21
 ********************************************************************/
#ifndef DB_H
#define DB_H

#include "inc/global.h"

#define SQL_MAXCOLUMN 100

extern int   db_init(char *servername, char *username, char *passwd);
extern int   db_insert(mail_t *mail, char *keyword, int type,
                       unsigned short port);
extern char* db_select(int type, int keytype, int *num);
extern void  db_close();

#endif /* DB_H */
