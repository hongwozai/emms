/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：unitest.c
 ** 创建人：路泽亚
 ** 描  述：单元测试的实现
 ** 用  法：1.简单方法，unitest中直接使用断言
 **        2.suite方法，先在TEST()中使用断言，
 **          在unitest中TEST_ADD添加TEST()过的函数即可
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-10-07
 ********************************************************************/
#ifndef UNITEST_H
#define UNITEST_H

#include <stdio.h>
#include <string.h>

#include "term.h"

extern unsigned int g_cnt_suite; /* 测试suite(函数)的个数 */
extern unsigned int g_cnt_test;  /* 测试单元的个数 */
extern unsigned int g_cnt_success; /* 运行成功的个数 */
extern unsigned int g_cnt_failure; /* 运行失败的个数 */

/* 测试链头结点 */
extern struct test_node g_test_chain;
extern struct test_node *g_chain_p;
/**
 * unitest 单元测试运行的主函数
 */
extern void unitest(void);

/**
 * 定义测试链节点结构，以及测试函数(测试函数无参)
 */
typedef void (*test_func)(void);
struct test_node {
    test_func func;
    const char *name;
    struct test_node *next;
};


#define TEST_SYM_NAME(NAME) TEST_##NAME
#define TEST_FUNC_NAME(NAME)  TESTFUNC_##NAME

#define TEST(NAME)                              \
    void TESTFUNC_##NAME (void);                \
    struct test_node TEST_SYM_NAME(NAME) =      \
    {TEST_FUNC_NAME(NAME), #NAME, NULL};        \
    void TEST_FUNC_NAME(NAME)(void)

#define TEST_ADD(NAME)                          \
    g_chain_p->next = &TEST_SYM_NAME(NAME);     \
    g_chain_p = g_chain_p->next;                \
    g_cnt_suite++;


/**
 * 所有关于断言的宏
 */
#define FALSE_PRINT(str, args...)                                   \
    printf(RED_STR("[%4d]:\t"str" Failure.\n"), __LINE__, ##args)

#define TRUE_PRINT(str, args...)                                    \
    printf(GREEN_STR("[%4d]:\t"str" Passed.\n"), __LINE__, ##args)

#define ASSERT_EQ(one, other)                       \
    g_cnt_test++;                                   \
    if ((one) == (other)){                          \
        TRUE_PRINT("%s == %s !", #one, #other);     \
        g_cnt_success++;                            \
    } else {                                        \
        FALSE_PRINT("%s == %s !", #one, #other);    \
        g_cnt_failure++;                            \
    }

#define ASSERT_EQ_RET(one, other)                   \
    g_cnt_test++;                                   \
    if ((one) == (other)){                          \
        TRUE_PRINT("%s == %s !", #one, #other);     \
        g_cnt_success++;                            \
        return;                                     \
    } else {                                        \
        FALSE_PRINT("%s == %s !", #one, #other);    \
        g_cnt_failure++;                            \
    }

#define ASSERT_NEQ(one, other)                      \
    g_cnt_test++;                                   \
    if ((one) != (other)){                          \
        TRUE_PRINT("%s != %s !", #one, #other);     \
        g_cnt_success++;                            \
    } else {                                        \
        FALSE_PRINT("%s != %s !", #one, #other);    \
        g_cnt_failure++;                            \
    }

/* 用来测试NULL，为空时返回 */
#define ASSERT_NEQ_RET(one, other)                  \
    g_cnt_test++;                                   \
    if ((one) != (other)){                          \
        TRUE_PRINT("%s != %s !", #one, #other);     \
        g_cnt_success++;                            \
    } else {                                        \
        FALSE_PRINT("%s != %s !", #one, #other);    \
        g_cnt_failure++;                            \
        return;                                     \
    }

#define ASSERT_GREATER(one, other)              \
    g_cnt_test++;                               \
    if ((one) > (other)){                       \
        TRUE_PRINT("%s > %s !", #one, #other);  \
        g_cnt_success++;                        \
    } else {                                    \
        FALSE_PRINT("%s > %s !", #one, #other); \
        g_cnt_failure++;                        \
    }

#define ASSERT_GREATER_EQ(one, other)               \
    g_cnt_test++;                                   \
    if ((one) >= (other)){                          \
        TRUE_PRINT("%s >= %s !", #one, #other);     \
        g_cnt_success++;                            \
    } else {                                        \
        FALSE_PRINT("%s >= %s !", #one, #other);    \
        g_cnt_failure++;                            \
    }

#define ASSERT_LESS(one, other)                 \
    g_cnt_test++;                               \
    if ((one) < (other)){                       \
        TRUE_PRINT("%s < %s !", #one, #other);  \
        g_cnt_success++;                        \
    } else {                                    \
        FALSE_PRINT("%s < %s !", #one, #other); \
        g_cnt_failure++;                        \
    }

#define ASSERT_LESS_EQ(one, other)                  \
    g_cnt_test++;                                   \
    if ((one) <= (other)){                          \
        TRUE_PRINT("%s <= %s !", #one, #other);     \
        g_cnt_success++;                            \
    } else {                                        \
        FALSE_PRINT("%s <= %s !", #one, #other);    \
        g_cnt_failure++;                            \
    }


#define ASSERT_STR_EQ(str, other)                   \
    g_cnt_test++;                                   \
    if (strcmp(str, other) == 0){                   \
        TRUE_PRINT("%s == %s !", #str, #other);     \
        g_cnt_success++;                            \
    } else {                                        \
        FALSE_PRINT("%s == %s !", #str, #other);    \
        g_cnt_failure++;                            \
    }

#define ASSERT_NSTR_EQ(str, other, N)                   \
    g_cnt_test++;                                       \
    if (strncmp(str, other, N) == 0){                   \
        TRUE_PRINT("%s %d== %s !", #str, N, #other);    \
        g_cnt_success++;                                \
    } else {                                            \
        FALSE_PRINT("%s %d== %s !", #str, N, #other);   \
        g_cnt_failure++;                                \
    }


#if 0
} /* 为了让cc-mode的缩进正常 */
#endif

#endif /* UNITEST_H */
