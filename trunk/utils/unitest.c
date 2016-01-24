/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：unitest.c
 ** 创建人：路泽亚
 ** 描  述：单元测试头文件
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-10-07
 ********************************************************************/
#include  "unitest.h"

unsigned int g_cnt_suite = 0; /* 测试suite(函数)的个数 */
unsigned int g_cnt_test = 0;  /* 测试单元的个数 */
unsigned int g_cnt_success = 0; /* 运行成功的个数 */
unsigned int g_cnt_failure = 0; /* 运行失败的个数 */

/* 测试链头结点 */
struct test_node g_test_chain = {unitest, "unitest", NULL};
struct test_node *g_chain_p;


/**
 * 运行测试链，第一个要运行unitest函数
 * @return 没有错误返回0， 有错误返回1
 */
int unitest_run()
{
    struct test_node *p;

    g_chain_p = &g_test_chain;
    for (p = &g_test_chain; p != NULL; p = p->next) {
        if (p == &g_test_chain) {
            printf("%s\n", p->name);
            printf("=============================================\n");
            p->func();
            continue;
        }
        printf("********************************\n");
        printf("%s\n", p->name);
        printf("********************************\n");
        p->func();
    }
    return g_cnt_failure != 0;
}

/**
 * 报告最后结果，并显示成功与否
 */
void unitest_report()
{
    int i;

    printf("\n=============================================\n");
    for (i = 0; i < 45; i++) {
        printf(g_cnt_failure?RED_BG(" "):GREEN_BG(" "));
    }
    printf("\n");
    printf("suite: %u; total: %u. success: %u, failure: %u\n",
           g_cnt_suite, g_cnt_test, g_cnt_success, g_cnt_failure);
}

#ifndef HAVE_MAIN
int main(void)
{
    int flag;

    setvbuf(stdout, NULL, _IONBF, 0);
    flag = unitest_run();
    unitest_report();
    return flag;
}
#endif
