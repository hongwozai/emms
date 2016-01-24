/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：systemlog.h
 ** 创建人：路泽亚
 ** 描  述：日志头文件
 ** 注  意：1.NDEBUG宏控制debug的显示，取消定义则显示（默认是定义的）
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-10-09
 ********************************************************************/
#ifndef MY_SYSTEM_LOG_H
#define MY_SYSTEM_LOG_H

#include <errno.h>
#include <string.h>

#include "term.h"

/* 日志文件的最大输出长度 */
#define LOG_MAXLINE 10240

/**
 * fatal,error,warn,info一定输出
 * debug视NDEBUG宏来判断是否输出, 默认输出到stdout
 * fatal用于致命错误, 默认会向stderr输出
 * error用于可恢复错误, 默认会向stderr输出
 * warn用于潜在错误（暂时不使用） , 默认会向stderr输出
 * info用于向日志输出固定信息, 默认会向stdout
 */
enum {
    LEVEL_DEBUG = 0,
    LEVEL_FATAL,
    LEVEL_ERROR,
    LEVEL_WARN,
    LEVEL_INFO
};

enum {
    /* 默认screen，stdout或者stderr */
    OUT_SCREEN = 0,
    OUT_LOGFILE
};

/**
 * 五种等级的宏定义
 */
#define FATAL(str, args...)                     \
    systemlog(LEVEL_FATAL, str"\n", ##args)

#define ERROR_NO(str, args...)                                      \
    systemlog(LEVEL_ERROR, RED_STR(str"(%s)\n"), ##args, strerror(errno))

#define ERROR(str, args...)                     \
    systemlog(LEVEL_ERROR, RED_STR(str"\n"), ##args)

#define INFO(str, args...)                      \
    systemlog(LEVEL_INFO, str"\n", ##args)

#define WARN(str, args...)                      \
    systemlog(LEVEL_WARN, str"\n", ##args)

#ifndef NDEBUG
#define DEBUG(str, args...)                             \
    systemlog(LEVEL_DEBUG, "%s:%s:%d\n"BLUE_STR(str), __FILE__,   \
              __FUNCTION__, __LINE__, ##args)
#else
#define DEBUG(str, args...) (void)(str)
#endif

/**
 * 系统日志的初始化，决定输出到什么地方
 * @param where 选择日志输出类型, 有输出到屏幕或输出到文件
 * @param  ...  where选择屏幕，则此参数忽略；选择文件，则此参数为文件名
 * @return 返回成功与否，成功返回0， 失败返回-1
 */
extern int systemlog_init(int where, ...);

/**
 * 输出系统日志
 * @param level 日志等级
 * @param print 输出信息，同printf格式一样
 * @param  ... 可选参数
 * @return 输出显示的字符数(print 和可选参数输出的总字符数，不算日期)
 */
extern int systemlog(int level, const char *print, ...);

/**
 * 系统日志的关闭，如果是选择屏幕，那么无所谓；
 * 如果是文件，需要调用该函数进行关闭文件
 */
extern void systemlog_close();

#endif /* MY_SYSTEM_LOG_H */
