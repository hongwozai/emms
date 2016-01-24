/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：monitor.c
 ** 创建人：路泽亚
 ** 描  述：监测模块，从监测队列中取，并放入相应的日志队列
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-22
 ********************************************************************/
#include <assert.h>
#include <unistd.h>
#include <pthread.h>

#include "inc/log.h"
#include "inc/spam.h"
#include "utils/acq.h"
#include "inc/match.h"
#include "inc/global.h"
#include "inc/monitor.h"
#include "utils/systemlog.h"

#define MONITOR_MSG_LEN 128
#define NODE_T struct monitor_msg_node

#define MAIL_NEXT(text, mail, i)                    \
    do {                                            \
        switch((i)) {                               \
        case 0: (text) = (mail)->from;              \
            break;                                  \
        case 1: (text) = (mail)->to;                \
            break;                                  \
        case 2: (text) = (mail)->subject;           \
            break;                                  \
        case 3: (text) = (mail)->content;           \
            break;                                  \
        case 4: (text) = (mail)->attach_name;       \
            break;                                  \
        case 5: (text) = (mail)->attach_content;    \
            break;                                  \
        default:                                    \
            (text) = NULL;                          \
        } } while (0);

Acq_T *monitor_queue[2];
static pthread_t monitor[2];

struct monitor_msg_node {
    mail_t *mail;
    unsigned short port;
};

static void* monitor_callback(void*);
static void  monitor_mail(NODE_T *node, int proto);

int monitor_init(void)
{
    monitor_queue[PROTO_WEB] = Acq_new(sizeof(NODE_T), MONITOR_MSG_LEN);
    if (!monitor_queue[PROTO_WEB]) {
        return -1;
    }
    monitor_queue[PROTO_NOWEB] = Acq_new(sizeof(NODE_T), MONITOR_MSG_LEN);
    if (!monitor_queue[PROTO_NOWEB]) {
        Acq_free(monitor_queue[PROTO_WEB]);
        return -1;
    }
    return 0;
}

void monitor_stop(void)
{
    int flag, i;

    for (i = 0; i < 2; i++) {
        if ((flag = pthread_cancel(monitor[i]))) {
            ERROR("thread monitor web_thread can't cancel(%s)!",
                  strerror(flag));
        }
        pthread_join(monitor[i], NULL);
    }
    monitor_close();
}

void monitor_close(void)
{
    Acq_free(monitor_queue[PROTO_WEB]);
    Acq_free(monitor_queue[PROTO_NOWEB]);
}

void monitor_run(void)
{
    long i;
    int flag;

    for (i = 0; i < 2; i++) {
        if ((flag = pthread_create(&monitor[i], NULL, monitor_callback,
                                   (void*)i))) {
            ERROR("thread monitor web_thread can't create!(%s)", strerror(flag));
            return;
        }
    }
}

int monitor_push(Acq_T *mcq, mail_t *mail, unsigned short port)
{
    int i;
    NODE_T msg;

    msg.mail    = mail;
    msg.port    = port;
    for (i = 0; i < 2; i++) {
        if (!Acq_enq(mcq, &msg))
            break;
        usleep(10);
    }

    return 0;
}

static void* monitor_callback(void* data)
{
    int proto = (int)(long)data;

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    for (;;) {
        NODE_T msg;

        Acq_deq(monitor_queue[proto], &msg);
        assert(msg.mail);
        /* 监测邮件 */
        monitor_mail(&msg, proto);
    }
    pthread_exit(NULL);
}

static void monitor_mail(NODE_T *node, int proto)
{
    int i = 0, flag = 0, monitor = 0, spam = 0;
    char *keyword;
    char *text = node->mail->from;

    for (i = 0; i < 6; i++) {
        /* 当mail中的不为空时才匹配 */
        if (text != NULL && *text != '\0') {
            keyword = match_match(match_use[TYPE_MONITOR][i], text);
            if (keyword) {
                DEBUG("text: %s\nkeyword: %s\n", text, keyword);
                monitor = 1;
                break;
            }
        }
        MAIL_NEXT(text, node->mail, i + 1);
    }
    if (node->mail->content) {
        /* 需要进行垃圾邮件的检测 */
        DEBUG("spam start....\n");
        flag = spam_filter(node->mail->content);
        if (flag == 1) {
            DEBUG("have spam!!!!!\n");
            /* 因为前台和数据库的问题只能先赋值 */
            spam = 1;
        }
    }
    if (monitor == 1 && spam == 0) {
        log_push(log_queue[proto][TYPE_MONITOR], node->mail,
                 keyword, node->port);
        return;
    }
    if (monitor == 0 && spam == 1) {
        log_push(log_queue[proto][TYPE_SPAM], node->mail,
                 keyword, node->port);
        return;
    }
    if (monitor == 1 && spam == 1) {
        log_push(log_queue[proto][TYPE_MONITOR_SPAM], node->mail,
                 keyword, node->port);
        return;
    }
    /* 如果能到达这里，说明不是匹配，不是封堵，不是垃圾邮件，那么释放邮件 */
    DEBUG("free null mail!\n");
    if (node->mail->content) {
        FREE(node->mail->content);
        node->mail->content = NULL;
    }
    if (node->mail->attach_content) {
        FREE(node->mail->attach_content);
        node->mail->attach_content = NULL;
    }
    Mpool_free(mailpool, node->mail);
    node->mail = NULL;
}
