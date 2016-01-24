/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：match.h
 ** 创建人：路泽亚
 ** 描  述：模式匹配部分头文件
 ** 注  意：1.本文件的实现全部采用char*形式，以\0结尾，
 **          不能用于utf-16等编码中有\0的编码方式
 **        2.该实现使用int作为长度，仅支持2^31长字符串
 **        3.创建trie树的两种方法
 **          一是使用make_trie,并向其传入字符串链表(使用list_push将字符串压入链表)
 **          二是使用trie_insert一个一个字符串插入到树中
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-10-04
 ********************************************************************/
#ifndef UTILS_MATCH_H
#define UTILS_MATCH_H

typedef struct am am_t;


/**
 * 拷贝到指定字符的数据并加\0，并返回指向该字符的指针
 * @param data 要拷贝的数据
 * @param buf  要拷贝的地方，一定要分配过内存
 * @param c    指定的字符
 * @return 返回指向c的指针
 */
extern char * strcpychr(const char *data, char *buf, char c);

/**
 * 分析出到指定字符串表示的数字，并返回指向该字符的指针
 * @param data
 * @param c
 * @param num
 * @return 返回指向字符c的指针
 */
extern char* strchr_parseint(char *data, char c, int *num);

/**
 * 将字符串形式表示的16进制数字转换为int类型，并返回指向该字符的指针
 * @param data
 * @param c
 * @param num
 * @return
 */
extern char* strchr_parse16(char *data, char c, int *num);

/**
 * 将一个字符表示的16进制数字转换为int类型
 * @param s
 * @return 返回转换的值（！！！未做错误处理，所以一定要保证输入的正确性）
 */
extern int char16toint(char *s, int len);


/**
 * 判断是否为txt文件
 * @param filename
 * @return 返回真(1)或假(0)
 */
extern int istxt(char *filename);

/**
 * 使用kmp算法的匹配函数
 * @param string 目标字符串，需要匹配的字符串
 *               由于使用\0作结尾，utf-16编码无法匹配
 * @param pattern 模式字符串
 * @param char   匹配到目标字符串的那个字符
 * @return 找不到子串返回NULL，找到子串返回子串的位置
 */
extern char *kmp_match(const char *, const char *, char);

/**
 * 创建一个新的自动机
 * @param int 预先创建的关键词空间大小
 * @return 返回非空成功,NULL为失败
 */
extern am_t* am_new(int);

/**
 * 销毁自动机
 * @param struct am *
 */
extern void am_destroy(am_t *);

/**
 * 向自动机中添加单词
 * @param struct am *
 * @param  char *
 * @return 返回非0失败（内存不够才会失败）
 */
extern int am_addword(am_t *, char *);

/**
 * 构建自动机
 * @param am
 * @return 返回0成功，返回-1失败（只有内存不够才会分配失败）
 */
extern int am_build(am_t*);

/**
 * 进行匹配
 * @param struct am *
 * @param  const char *
 * @return 返回匹配的关键字,返回NULL为失败
 */
extern char* am_match(struct am *, const char *);

/**
 * 重建自动机，添加删除都要重建，释放原本空间并重新分配关键字个数
 * @param struct am *
 * @param int 要创建的关键字个数
 * @return 返回-1失败（内存不够才失败）,返回0成功
 */
extern int am_rebuild(struct am *, int);

#endif /* UTILS_MATCH_H */
