/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：match.c
 ** 创建人：路泽亚
 ** 描  述：匹配模块的实现
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-09
 ********************************************************************/
#include <iconv.h>
#include <stdlib.h>
#include <assert.h>

#include "inc/db.h"
#include "inc/match.h"
#include "inc/global.h"
#include "utils/systemlog.h"

#define KILL_WORD_COUNT    10
#define MONITOR_WORD_COUNT 10
#define WORD_INDEX(word, i)                     \
    ((word) + SQL_MAXCOLUMN * (i))

/* 自动机的定义 */
/* 使用两个宏来选定自动机 */
am_t *match_use[2][6];

static void match_build_from_db(int type, int key_type, am_t *am);

/**
 * 创建自动机，并且从数据库中读取关键字添加到自动机中
 */
int match_init()
{
    int i, j;
    /* 创建自动机 */
    for (i = 0; i < 6; i++) {
        match_use[TYPE_KILL][i]    = am_new(KILL_WORD_COUNT);
        if (!match_use[TYPE_KILL][i]) {
            for (j = 0; j < i; j++) {
                am_destroy(match_use[TYPE_KILL][j]);
            }
            return -1;
        }
        match_use[TYPE_MONITOR][i] = am_new(MONITOR_WORD_COUNT);
        if (!match_use[TYPE_MONITOR][i]) {
            for (j = 0; j < i; j++) {
                am_destroy(match_use[TYPE_MONITOR][j]);
            }
            return -1;
        }
    }
    /* 添加关键字并重建自动机 */
    /* 测试 */
    for (i = 0; i < 2; i++)
        for (j = 0; j < 6; j++)
            match_build_from_db(i, j, match_use[i][j]);
    return 0;
}

void match_close()
{
    int i, j;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 6; j++) {
            am_destroy(match_use[i][j]);
        }
    }
}

char* match_match(am_t *use, const char *str)
{
    /* 自动机会直接返回一个自动机内部保存的变量，当自动机要销毁时，另一个线程
     * 还操作该内存就会出现问题 */
    return am_match(use, str);
}

void match_rebuild(int type, int key_type)
{
    am_t *tmp, *tmp1;
    int   num = (type == TYPE_KILL)?
        KILL_WORD_COUNT: MONITOR_WORD_COUNT;

    /* 先创建备用自动机，从数据库中读取数据，
     * 而后再将备用的与正在使用的进行交换，
     * 最后将换下来的销毁*/
    type     = (type > 1)? 1 : type;
    key_type = (key_type > 5)? 5 : key_type;
    /* 多线程情况下不能一次一次的构建，需要一起完成 */
    tmp = am_new(num);
    /* 从数据库中读取 */
    match_build_from_db(type, key_type, tmp);

    /* FIXME: 交换，这里有一个多线程的问题 */
    tmp1 = match_use[type][key_type];
    match_use[type][key_type] = tmp;
    am_destroy(tmp1);
}

/* 暂时用作测试 */
/* type, keytype 分别是 enum TYPE, enum KEY_TYPE */
static void match_build_from_db(int type, int key_type, am_t *am)
{
    int i;
#ifndef HAVE_DB
    for (i = 0; i < 6; i++) {
        am_addword(match_use[TYPE_KILL][i], "qq");
        am_addword(match_use[TYPE_KILL][i], "fuck");
        am_addword(match_use[TYPE_KILL][i], "shit");
        am_build(match_use[TYPE_KILL][i]);
        /* 监测关键字 */
        am_addword(match_use[TYPE_MONITOR][i], "xijinping");
        am_addword(match_use[TYPE_MONITOR][i], "likeqiang");
        am_addword(match_use[TYPE_MONITOR][i], "ccc");
        am_build(match_use[TYPE_MONITOR][i]);
    }
#else
    int num, flag;
    char *words;
    char gbkwordbuf[SQL_MAXCOLUMN];
    char *gbkword = gbkwordbuf;
    char *utfword;
    iconv_t trans;
    size_t utflen, gbklen = sizeof(gbkwordbuf);

    words = db_select(type, key_type, &num);
    if (!words) {
        DEBUG("Database have no data or error!\n");
        am_addword(am, "shit");
        am_build(am);
        return;
    }

    trans = iconv_open("GBK", "UTF-8");
    if (trans == NULL) {
        ERROR_NO("iconv create error!");
        return;
    }
    for (i = 0; i < num; i++) {
        assert(WORD_INDEX(words, i));
        DEBUG("keyword: %s\n", WORD_INDEX(words, i));
        am_addword(am, WORD_INDEX(words, i));

        /* 这里转换编码 */
        utfword = WORD_INDEX(words, i);
        utflen = strlen(utfword);
        gbkword = gbkwordbuf;
        gbklen = sizeof(gbkwordbuf);
        flag = iconv(trans, &utfword, &utflen, &gbkword, &gbklen);
        if (flag == -1) {
            ERROR_NO("iconv trans error!");
            continue;
        }
        gbkwordbuf[sizeof(gbkwordbuf) - gbklen] = '\0';
        /* iconv会修改utfword这个指针 */
        if (strlen(WORD_INDEX(words, i)) != (sizeof(gbkwordbuf) - gbklen)) {
            am_addword(am, gbkwordbuf);
        }
    }
    am_build(am);
    free(words);
    iconv_close(trans);
#endif
}
