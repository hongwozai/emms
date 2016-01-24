/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：debug.c
 ** 创建人：路泽亚
 ** 描  述：1.
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-26
 ********************************************************************/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <execinfo.h>

#include "debug.h"
#include "systemlog.h"

#define BACKTRACE_SIZ 1024

void do_backtrace()
{
    void *array[BACKTRACE_SIZ];
    size_t size, i;
    char **strings;

    size = backtrace(array, BACKTRACE_SIZ);
    strings = backtrace_symbols(array, size);
    if (!strings) {
        ERROR_NO("backtrace: ");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < size; i++) {
        fprintf(stderr, "%p: %s\n", array[i], strings[i]);
    }
    free(strings);
}

