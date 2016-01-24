/********************************************************************
 ** Copyright(c) 2016,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：fileutils.h
 ** 创建人：路泽亚
 ** 描  述：文件处理相关函数
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2016-01-17
 ********************************************************************/
#ifndef FILEUTILS_H
#define FILEUTILS_H

int walk_files(const char *dirname,
               void (*walk)(const char *filename));
int isdir(const char *dirname);

#endif /* FILEUTILS_H */
