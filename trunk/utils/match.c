/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：match.c
 ** 创建人：路泽亚
 ** 描  述：模式匹配的算法实现
 ** TODO: 1.ac自动机的内存占用太大
 **       2.ac自动机使用的内存分配策略必须更改
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-10-04
 ********************************************************************/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "match.h"

/* ========================== 字符串公共函数 ====================== */
/* 从data处开始拷贝数据到buf中，并加\0，拷贝直到c字符截止 */
/* 并返回指向c的指针 */
char *
strcpychr(const char *data, char *buf, char c)
{
    int  i;
    for (i = 0; *data != c; data++, i++) {
        buf[i] = *data;
    }
    buf[i] = '\0';
    return (char*)data;
}

char* strchr_parseint(char *data, char c, int *num)
{
    int i;
    char buf[10];
    for (i = 0; *data != c; data++, i++) {
        buf[i] = *data;
    }
    buf[i] = '\0';
    *num = atoi(buf);
    return data;
}

char* strchr_parse16(char *data, char c, int *num)
{
    int i;
    char buf[10];
    for (i = 0; *data != c; data++, i++) {
        buf[i] = *data;
    }
    buf[i] = '\0';
    *num = char16toint(buf, i);
    return data;
}

/* 将用字符表示的16进制数字转换为int */
/* 这里的s为结尾 */
int char16toint(char *s, int len)
{
    int i;
    int res = 0;

    for (i = 0; i < len; i++) {
        char tmp = *(s + (len - i) -1);
        tmp = tmp >= 'a' ? tmp - 87: tmp - '0';
        res += tmp * pow(16, i);
    }
    return res;
}


int istxt(char *filename)
{
    char *q;
    q = strrchr(filename, '.');
    if (q && !strcmp(q+1, "txt")) {
        return 1;
    }
    return 0;
}

/* ============================ kmp ============================= */
/**
 * 仅供kmp使用，构造next数组（调转表）
 * @param pattern 模式串
 * @param len 模式串的长度
 * @param next 构造出来的模式串的调转表, 其长度与pattern的长度一样(len)
 */
static void
kmp_mknext(const char *pattern, int len, int *next)
{
    int q;                      /* 模式串下标 */
    int k;                      /* 最大前后缀匹配长度 */

    next[0] = 0;                /* 第一个的最大前后缀匹配长度为0 */
    for (q = 1, k = 0; q < len; q++) {
        while (k > 0 && pattern[q] != pattern[k]) {
            k = next[k - 1];
        }
        if (pattern[q] == pattern[k]) {
            k++;
        }
        next[q] = k;
    }
}

char *
kmp_match(const char *str, const char *pattern, char c)
{
    int i, j = 0;
    int plen = strlen(pattern);
    int next[plen];

    kmp_mknext(pattern, plen, next);
    for (i = 0; str[i] != c; i++) {
        if (str[i] == pattern[j]) {
            j++;
        } else if (j == 0) {
            continue;
        } else {
            j = next[j];
        }
        if (j == plen) {
            return (char*)( str + i - plen + 1 );
        }
    }
    return NULL;
}

/* =================== ac automation =========================== */

/* 字符集最大个数 */
#define MAX_CHARS 256

struct am_keyword {
    char *keyword;
    int   len;
};

struct am {
    struct am_keyword *word_vector;
    int count_max;              /* word_vector的长度 */
    int word_count;             /* word_vector存了几个单词 */
    int words_len;              /* 所有单词的总长度 */
    int (*chart)[MAX_CHARS + 1];
};

struct am*
am_new(int size)
{
    struct am *am;

    size = (size > 2) ? size : 2;
    am = (struct am*)malloc(sizeof(struct am));
    if (!am) {
        return NULL;
    }
    am->word_count = 0;
    am->word_vector = (struct am_keyword*)malloc(sizeof(struct am_keyword)*size);
    if (!am->word_vector) {
        free(am);
        return NULL;
    }
    am->count_max = size;
    am->words_len = 0;
    am->chart     = NULL;
    return am;
}

void
am_destroy(struct am *am)
{
    int i;
    for (i = 0; i < am->word_count; i++) {
        free(am->word_vector[i].keyword);
    }

    if (am->word_vector) {
        free(am->word_vector);
    }
    if (am->chart) {
        free(am->chart);
    }
    free(am);
}

int
am_addword(struct am *am, char *word)
{
    int i;

    assert(am);
    assert(word);
    if (am->word_count == am->count_max) {
        /* wordvector满了,count_max翻倍，word_ */
        struct am_keyword *tmp;
        am->count_max += am->count_max;
        tmp = calloc(am->count_max, sizeof(struct am_keyword));
        if (!tmp) {
            return -1;
        }
        for (i = 0; i < am->word_count; i++) {
            tmp[i] = am->word_vector[i];
        }
        free(am->word_vector);
        am->word_vector = tmp;
    }
    /* 将单词复制 */
    am->word_vector[am->word_count].keyword = malloc(strlen(word) + 1);
    strcpy(am->word_vector[am->word_count].keyword, word);
    am->word_vector[am->word_count].len     = strlen(word);
    am->words_len += am->word_vector[am->word_count].len;
    am->word_count++;
    return am->word_count;
}

int
am_build(struct am *am)
{
    int i, j;
    int k, n;                   /* k上一个分配空间，n当前状态 */
    unsigned char c;
    int *queue;
    int head, tail;             /* 队列头尾 */
    int *fail;                  /* 失败指针 */
    int tmp;

    assert(am);
    free(am->chart);
    /* 注意分配时要有一个根节点的指针，所以加1 */
    am->chart = calloc(am->words_len + 1, sizeof(int) * (MAX_CHARS + 1));
    if (!am->chart) {
        return -1;
    }
    if (am->word_count == 0) {
        return -1;
    }
    /* 总共分为两步：
     * 1.构建树
     * 2.构建失败指针
     */
    /* 构建树 */
    k = 0;
    for (i = 0; i < am->word_count; i++) {
        n = 0;
        for (j = 0; j < am->word_vector[i].len; j++) {
            c = am->word_vector[i].keyword[j];
            if (am->chart[n][c] == 0) {
                am->chart[n][c] = ++k;
                n = k;
            } else {
                n = am->chart[n][c];
            }
        }
        am->chart[n][MAX_CHARS] = i + 1;
    }
    /* 构建失败指针,BFS方式 */
    fail = (int*)malloc(sizeof(int) * am->words_len);
    if (!fail) {
        free(am->chart);
        return -1;
    }
    queue = (int*)malloc(sizeof(int) * am->words_len);
    if (!queue) {
        free(am->chart);
        free(fail);
        return -1;
    }
    head = tail = 0;
    for (i = 0; i < MAX_CHARS; i++) {
        /* 所有第二层节点失败指针为0 */
        if (am->chart[0][i]) {
            fail[tail] = 0;
            queue[tail++] = am->chart[0][i];
        }
    }
    while (head != tail) {
        for (i = 0; i < MAX_CHARS; i++) {
            if (am->chart[queue[head]][i]) {
                tmp = am->chart[queue[head]][i];
                queue[tail] = tmp;
                fail[tail]  = am->chart[fail[head]][i];
                tail = (tail + 1) % (am->words_len);
            } else {
                /* 如果该子节点为空，那么直接将该节点指向父节点失败指针对应的子节点 */
                tmp = am->chart[fail[head]][i];
                am->chart[queue[head]][i] = tmp;
            }
        }
        head = (head + 1) % (am->words_len);
    }
    free(queue);
    free(fail);
    return 0;
}

char*
am_match(struct am *am, const char *str)
{
    int state = 0;
    const char *p = str;

    assert(am);
    assert(str);
    while (*p) {
        state = am->chart[state][(unsigned char)*p];
        if (am->chart[state][MAX_CHARS]) {
            return am->word_vector[am->chart[state][MAX_CHARS] - 1].keyword;
        }
        p++;
    }
    return NULL;
}

int
am_rebuild(struct am *am, int num)
{
    int i;

    num = (num > 2)? num : 2;
    /* 为NULL时释放不会出现错误 */
    free(am->chart);
    for (i = 0; i < am->word_count; i++) {
        free(am->word_vector[i].keyword);
    }
    free(am->word_vector);
    am->word_vector
        = (struct am_keyword*)calloc(num, sizeof(struct am_keyword));
    if (!am->word_vector) {
        return -1;
    }
    am->count_max  = num;
    am->words_len  = 0;
    am->chart      = NULL;
    am->word_count = 0;
    return 0;
}
