/********************************************************************
 ** Copyright(c) 2016,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：shash.h
 ** 创建人：路泽亚
 ** 描  述：垃圾邮件使用的哈希表
 ** 注  意：
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2016-01-17
 ********************************************************************/
#ifndef SHASH_H
#define SHASH_H

#define T Shash_T

typedef struct Shash Shash_T;

extern T*    Shash_new(int len, int size);
extern void  Shash_free(T *hash);
extern void* Shash_find(T *hash, char *key);
extern int   Shash_insert(T *hash, char *key, void *value);
extern int   Shash_del(T *hash, char *key);
extern void  Shash_rewalk(T *hash);
extern int   Shash_walk(T *hash, char **key, void **value);

#undef T
#endif /* SHASH_H */
