/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：debug.h
 ** 创建人：路泽亚
 ** 描  述：用于debug的函数与宏
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-20
 ********************************************************************/
#ifndef MY_UTILS_DEBUG_H
#define MY_UTILS_DEBUG_H

#include <time.h>
#include <sys/time.h>

/* benchmark */
#ifndef NDEBUG
#define BENCH(str)                                      \
    do{ struct timespec start, end;                       \
    char *string = str;                                          \
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

#define END                                                     \
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);              \
    printf("%s: %ld ns\n", string, end.tv_nsec - start.tv_nsec);   \
    } while (0);
#else
#define BENCH(str) (void)str;
#define END
#endif

/* backtrace */
extern void do_backtrace();

#endif /* MY_UTILS_DEBUG_H */
