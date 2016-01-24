/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：systemlog.c
 ** 创建人：路泽亚
 ** 描  述：系统日志部分的头文件
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-10-09
 ********************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "systemlog.h"

static int g_out_stderr = STDERR_FILENO;
static int g_out_stdout = STDOUT_FILENO;
static const char *g_level_str[] = {
    "DEBUG",                    /* LEVEL_DEBUG */
    "FATAL",                    /* LEVEL_FATAL */
    "ERROR",                    /* LEVEL_ERROR */
    "WARN",                     /* LEVEL_WARN */
    "INFO",                     /* LEVEL_INFO */
};

int systemlog_init(int where, ...)
{
    va_list ap;

    va_start(ap, where);
    if (where == OUT_LOGFILE) {
        const char *filename = va_arg(ap, const char*);
        int fileno = open(filename, O_WRONLY|O_CREAT|O_APPEND,
                          S_IRUSR|S_IWUSR);
        if (fileno == -1) {
            va_end(ap);
            return -1;
        }
        g_out_stderr = fileno;
        g_out_stdout = fileno;
    }
    va_end(ap);
    return 0;
}

int systemlog(int level, const char *print, ...)
{
    int ret = 0;
    int size;
    char buf[LOG_MAXLINE];
    va_list ap;
    time_t t;
    const char *level_str = g_level_str[level];
    int level_len = strlen(g_level_str[level]);

    va_start(ap, print);
    t = time(NULL);
    size = strftime(buf, LOG_MAXLINE, "[%F %T] ", localtime(&t));
    strcpy(buf + size, level_str);
    strcpy(buf + size + level_len, ": ");
    vsnprintf(buf + size + level_len + 2,
              LOG_MAXLINE - size - level_len - 2, print, ap);
    buf[LOG_MAXLINE - 2] = '\n';
    buf[LOG_MAXLINE - 1] = '\0';
    if (level == LEVEL_DEBUG || level == LEVEL_INFO) {
        ret = write(g_out_stdout, buf, strlen(buf));
    } else if (level == LEVEL_ERROR || level == LEVEL_FATAL ||
               level == LEVEL_WARN) {
        ret = write(g_out_stderr, buf, strlen(buf));
    } else {
        va_end(ap);
        return -1;
    }
    va_end(ap);
    return ret - size - level_len - 2;
}


void systemlog_close()
{
    if (g_out_stderr != STDERR_FILENO) {
        close(g_out_stderr);
    }
    if (g_out_stdout != STDOUT_FILENO) {
        close(g_out_stdout);
    }
}
