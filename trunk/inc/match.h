/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：match.h
 ** 创建人：路泽亚
 ** 描  述：匹配模块的头文件
 ** 注  意：1.初始化之前必须首先初始化db模块
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-09
 ********************************************************************/
#ifndef MATCH_H
#define MATCH_H

#include "utils/match.h"

/* 自动机，用于match_match函数中 */
/* 使用时不要直接使用数字，要用global.h中定义的宏enum KEY_TYPE
 * 和enum TYPE
 */
extern am_t *match_use[2][6];

/**
 * 初始化匹配模块
 * @return 返回0代表成功，非0代表失败
 */
extern int match_init();

/**
 * 匹配，直接传指定自动机来匹配字符串
 * @param use 要使用的自动机
 * @param str 要匹配的目标串
 * @return 返回匹配到的字符串，NULL为没有匹配到
 */
extern char *match_match(am_t *use, const char *str);

/**
 * 重建指定自动机，包括切换过程
 * @param type     enum TYPE 类型
 * @param key_type enum KEY_TYPE 类型
 */
extern void match_rebuild(int type, int key_type);

/**
 * 释放创建的自动机
 */
extern void match_close();

#endif /* MATCH_H */
