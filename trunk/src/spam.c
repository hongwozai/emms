/********************************************************************
 ** Copyright(c) 2016,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：spam.c
 ** 创建人：路泽亚
 ** 描  述：垃圾邮件模块
 ** 注  意：1.中间使用了静态变量,多线程情况下不能使用，除非限制到只有单线程操作
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2016-01-17
 ********************************************************************/
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "inc/spam.h"
#include "utils/shash.h"
#include "utils/systemlog.h"
#include "utils/fileutils.h"

#define MAX_NUM 10000
#define MAXLINE 1024
#define HASH_LEN 1024
#define MAX_WORD_LEN 20
#define MIN_WORD_LEN 1

#define DIR_HAM "ham"
#define DIR_SPAM "spam"

struct spam {
    /* 需要读取和写入的配置文件 */
    char *filename;
    double thresold;
    /* 垃圾邮件数，正常邮件数，需要从文件中读取 */
    unsigned spam_mail_num;
    unsigned ham_mail_num;
    unsigned total_mail_num;
    /* 垃圾邮件单词数，正常邮件单词数，需要从文件中读取 */
    unsigned spam_num;
    unsigned ham_num;
    unsigned total_num;
    /* 所有单词都存放在一起，不分good,bad表 */
    Shash_T *hash;
    /* 供训练时使用, 表是当前是ham or spam */
    int isspam;
};

struct spam_node {
    unsigned spam;
    unsigned ham;
    unsigned amount;
};

static struct spam g_spam;

static int   load_file();
static void  walk(const char *filename);
static char* word_process(char *word);
static char* get_token(FILE *fp);
static char* get_token_string(char *content, char *word);

int spam_init(double thresold, char *filename)
{
    g_spam.thresold  = thresold;
    g_spam.spam_num  = 0;
    g_spam.ham_num   = 0;
    g_spam.total_num = 0;
    g_spam.spam_mail_num = 0;
    g_spam.ham_mail_num  = 0;
    g_spam.total_mail_num = 0;
    g_spam.filename = filename;
    g_spam.hash = Shash_new(HASH_LEN, sizeof(struct spam_node));
    if (!g_spam.hash) {
        return -1;
    }
    if (-1 == load_file()) {
        Shash_free(g_spam.hash);
        return -1;
    }
    return 0;
}

/* 目录下要有ham与spam两个目录 */
int spam_train(const char *dir)
{
    char tmp[1024];

    /* 读取每一个文件，记录单词频率 */
    sprintf(tmp, "%s/%s", dir, DIR_HAM);
    g_spam.isspam = 0;
    walk_files(tmp, walk);

    sprintf(tmp, "%s/%s", dir, DIR_SPAM);
    g_spam.isspam = 1;
    walk_files(tmp, walk);
    return 0;
}

#define MAIL_WORD_MAX 1024
static char filter_word[MAIL_WORD_MAX];

/* 输入content内容，返回spam(1)与ham(0) */
/**
 * http://blog.csdn.net/sdf000/article/details/17020367
 * 学习的网址
 *
 * 算法过程：（由于算数下溢，需要取对数进行计算）
 * 先计算hash表中，一个给定单词出现时这封邮件是垃圾邮件的概率
 * 计算方法就是使用贝叶斯公式，p(s|w) = p(w|s)p(s) / p(w|s)p(s) + p(w|h)p(h)
 * 其中p(s), p(h)是统计学中垃圾与非垃圾的概率，在无偏差的情况下，可以将其都设为0.5
 * 而后计算整封邮件是垃圾邮件的概率
 */
int spam_filter(char *content)
{
    char *key, *p = content;
    Shash_T *test;
    double pn, prob_spam = 0;
    struct spam_node node;
    struct spam_node *value;

    assert(content);
    test = Shash_new(128, sizeof(struct spam_node));
    while ((p = get_token_string(p, filter_word))) {
        char *word = word_process(filter_word);
        if (!word)
            continue;
        Shash_insert(test, word, &node);
    }
    Shash_rewalk(g_spam.hash);
    while (-1 != Shash_walk(g_spam.hash, &key, (void**)&value)) {
        if (Shash_find(test, key)) {
            if (value->spam != 0) {
                pn = (double)1 /
                    (1 +
                     ((double)value->ham * g_spam.spam_mail_num) /
                     ((double)value->spam * g_spam.ham_mail_num));
                if (pn == 0) {
                    pn = 0.01;
                } else if (pn == 1){
                    pn = 0.99;
                }
                prob_spam += log10(1 - pn) - log10(pn);
            }
        }
    }

    prob_spam = 1 / (pow(10, prob_spam) + 1);
    if (prob_spam > g_spam.thresold) {
        Shash_free(test);
        return 1;
    } else {
        Shash_free(test);
        return 0;
    }
}

void spam_close()
{
    FILE *fp;
    char *key;
    char tmp[MAXLINE];
    struct spam_node *value;

    if (!(fp = fopen(g_spam.filename, "w+"))) {
        ERROR_NO("%s can't not open!", g_spam.filename);
        return;
    }
    /* 写回文件中,先写邮件数，再写单词频率 */
    sprintf(tmp, "%u %u %u\n", g_spam.spam_num, g_spam.ham_num,
            g_spam.total_num);
    fputs(tmp, fp);
    sprintf(tmp, "%u %u %u\n", g_spam.spam_mail_num, g_spam.ham_mail_num,
            g_spam.total_mail_num);
    fputs(tmp, fp);
    while (-1 != Shash_walk(g_spam.hash, &key, (void**)&value)) {
        sprintf(tmp, "%s %u %u %u\n", key, value->spam, value->ham,
                value->amount);
        fputs(tmp, fp);
    }
    fclose(fp);
    Shash_free(g_spam.hash);
}

/* ============== 分类部分 ================= */
/* 从文件中读取而后插入hash表 */
/* 文件格式：
 * 行：单词 spam ham amount */
static int load_file()
{
    FILE *fp;
    unsigned spam;
    unsigned ham;
    unsigned amount;
    char buf[MAXLINE];
    char word[MAX_WORD_LEN + 1];
    struct spam_node node;

    if (!(fp = fopen(g_spam.filename, "r"))) {
        ERROR_NO("%s can't not open!", g_spam.filename);
        return -1;
    }
    /* 加载单词数 */
    fgets(buf, MAXLINE, fp);
    sscanf(buf, "%11u %11u %11u\n", &g_spam.spam_num, &g_spam.ham_num,
           &g_spam.total_num);
    /* 加载邮件数 */
    fgets(buf, MAXLINE, fp);
    sscanf(buf, "%11u %11u %11u\n", &g_spam.spam_mail_num, &g_spam.ham_mail_num,
           &g_spam.total_mail_num);
    while (fgets(buf, MAXLINE, fp)) {
        sscanf(buf, "%20s %11u %11u %11u\n", word, &spam, &ham, &amount);
        node.ham    = ham;
        node.spam   = spam;
        node.amount = amount;
        Shash_insert(g_spam.hash, word, &node);
    }
    fclose(fp);
    return 0;
}

static char* get_token_string(char *content, char *word)
{
    int i, j;
    char *ret = content;

    for (i = 0, j = 0; *content != '\0'; ++content, ++j) {
        if (!isalnum(*content)) {
            if (i == 0)
                continue;
            ++j;
            break;
        }
        word[i++] = *content;
    }
    word[i] = '\0';
    ret += j;
    if (*content == '\0')
        return NULL;
    return ret;
}

/* ============== 训练部分 ================= */
/* 获取文件中的单词，而后查找hash表,
 * 有该项则将对应的ham或spam添加一，总数也加一
 * 无该项则创建这一项，而后全部初始话为0,再将对应项加一*/
static void walk(const char *filename)
{
    FILE *fp;
    char *word;
    struct spam_node node;

    assert(filename);
    if (!(fp = fopen(filename, "r"))) {
        ERROR_NO("%s can't open!", filename);
        return;
    }
    g_spam.isspam ? ++g_spam.spam_mail_num : ++g_spam.ham_mail_num;
    ++g_spam.total_mail_num;
    while ((word = get_token(fp))) {
        struct spam_node *nodep;
        nodep = Shash_find(g_spam.hash, word);
        if (nodep) {
            g_spam.isspam ? ++nodep->spam : ++nodep->ham;
            ++nodep->amount;
        } else {
            memset(&node, 0, sizeof(struct spam_node));
            g_spam.isspam ? ++node.spam : ++node.ham;
            ++node.amount;
            Shash_insert(g_spam.hash, word, &node);

            /* 单词数的增加 */
            g_spam.isspam ? ++g_spam.spam_num : ++g_spam.ham_num;
            ++g_spam.total_num;
        }
    }
    fclose(fp);
}

static char* word_process(char *word)
{
    int i, len;

    assert(word);
    len = strlen(word);
    if (len <= MIN_WORD_LEN || len >= MAX_WORD_LEN) {
        return NULL;
    }
    for (i = 0; i < len; i++) {
        word[i] = tolower(word[i]);
    }
    return word;
}

/* 带缓冲的get_token, g_bool为1时，需要从文件中读取 */
/* 这个长度根据各个邮件中最长的行来决定，这样速度快，
 * 但是如果超过会出现realloc的错误,因为getline会realloc
 * 提供的内存
 * 10万字节应该够用 */
#define MAIL_MAXLINE 102400
static int g_bool = 1;
static char g_line[MAIL_MAXLINE];

/* 返回的内存是全局静态变量的，所以不用释放，但是下一次使用之前
 * 必须赋值出来否则很可能无效 */
static char* get_token(FILE *fp)
{
    ssize_t ret;
    char *word = NULL;
    char *linep = NULL;
    size_t linelen = MAIL_MAXLINE;

    do {
        if (g_bool == 1) {
            linep = g_line;
            linelen = MAIL_MAXLINE;
            ret = getline(&linep, &linelen, fp);
            if (ret == -1) {
                return NULL;
            }
            g_bool = 0;
        }
        word = strtok(linep, " -:,.'/\r\n)?");
        if (word) {
            /* 在这里是这一行获取到单词，但是可能不符合条件的 */
            linep = NULL;
            word = word_process(word);
        } else {
            g_bool = 1;
        }
    } while(!word);
    return word;
}
