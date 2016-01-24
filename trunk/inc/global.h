/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：global.h
 ** 创建人：路泽亚
 ** 描  述：全局变量或全局宏
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-11-24
 ********************************************************************/
#ifndef GLOBAL_H
#define GLOBAL_H

#include "utils/mem.h"
#include "utils/tpool.h"

#define POP3    110
#define IMAP    143
#define SMTP    25
#define WEBMAIL 80

#define POOL_BLOCK 10240

#define MAIL_INIT(mail)                         \
    do {                                        \
        (mail)->from[0] = '\0';                 \
        (mail)->to[0]   = '\0';                 \
        (mail)->subject[0] = '\0';              \
        (mail)->content = NULL;                 \
        (mail)->attach_name[0] = '\0';          \
        (mail)->attach_content = NULL;          \
    } while(0);

enum PROTO {
    PROTO_WEB = 0,
    PROTO_NOWEB
};

enum TYPE {
    TYPE_KILL = 0,
    TYPE_MONITOR,
    TYPE_SPAM,
    TYPE_MONITOR_SPAM,
};

enum KEY_TYPE {
    KEY_FROM = 0,
    KEY_TO,
    KEY_SUBJECT,
    KEY_CONTENT,
    KEY_ATTACH_NAME,
    KEY_ATTACH_CONTENT
};

typedef struct mail {
    char from[128];
    char to[128];
    char subject[128];
    char *content;
    char attach_name[128];
    char *attach_content;
} mail_t;

/* 由于两个协议的分析，以及日志模块都要使用该邮件模块，所以定义为全局 */
/* 在分析模块分配与释放，对其中邮件节点的内存释放由日志模块来 */
extern Mpool_T   *mailpool;
extern Tpool_T   *threadpool;

#endif /* GLOBAL_H */
