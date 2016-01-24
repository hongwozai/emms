/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：log.c
 ** 创建人：路泽亚
 ** 描  述：日志模块
 ** 注  意：
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-22
 ********************************************************************/
#include <unistd.h>
#include <pthread.h>

#include "inc/db.h"
#include "inc/log.h"
#include "inc/global.h"
#include "utils/systemlog.h"

#define LOG_MSG_LEN 128
#define NODE_T struct log_node

struct log_node {
    mail_t *mail;
    char *keyword;
    unsigned short port;
};

/**
 * 两个都用枚举来表示
 * enum PROTO enum TYPE
 */
Acq_T *log_queue[2][4];
static pthread_t log[2][3];

static void* log_callback(void *data);
static int   log_record(NODE_T *node, int type);

int log_init()
{
    int i, j;
    for (i = 0; i < 2; i++)
        for (j = 0; j < 4; j++) {
            log_queue[i][j] = Acq_new(sizeof(NODE_T), LOG_MSG_LEN);
            /* FIXME: 这里如果失败会有泄漏，但是写着有点麻烦，先放到这里吧 */
            if (!log_queue[i][j])
                return -1;
        }
    return 0;
}

void log_stop()
{
    int flag, i, j;

    for (i = 0; i < 2; i++)
        for (j = 0; j < 4; j++) {
            if ((flag = pthread_cancel(log[i][j]))) {
                ERROR("thread log can't cancel(%s)!", strerror(flag));
            }
            pthread_join(log[i][j], NULL);
        }
    log_close();
}

void log_close(void)
{
    int i, j;
    for (i = 0; i < 2; i++)
        for (j = 0 ; j < 4; j++)
            Acq_free(log_queue[i][j]);
}

void log_run()
{
    long type;
    int flag, i, j;

    for (i = 0; i < 2; i++)
        for (j = 0; j < 4; j++) {
            type = (i << 2) + j;
            if ((flag = pthread_create(&log[i][j], NULL, log_callback,
                                       (void*)type))) {
                ERROR("thread log can't create!(%s)", strerror(flag));
                return;
            }
        }
}

int log_push(Acq_T *mcq, mail_t *mail, char *keyword,
             unsigned short port)
{
    NODE_T msg;

    msg.mail    = mail;
    msg.keyword = keyword;
    msg.port    = port;
    while (Acq_enq(mcq, &msg))
        usleep(5);
    return 0;
}

static void* log_callback(void *data)
{
    int proto = ((unsigned long)data >> 2);
    int type  = ((unsigned long)data & 0x03);

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    for (;;) {
        NODE_T msg;

        Acq_deq(log_queue[proto][type], &msg);
        log_record(&msg, type);
    }
    pthread_exit(NULL);
}

static int log_record(NODE_T *node, int type)
{
#ifndef HAVE_DB
    DEBUG("log: type %d\nkeyword: %s\nfrom: %s\nto: %s\nsubject: %s\n"
          "content: %s\nattachname: %s\nattachcontent: %s\n",
          type, node->keyword, node->mail->from, node->mail->to,
          node->mail->subject, node->mail->content, node->mail->attach_name,
          node->mail->attach_content);
#else
    if (type == TYPE_SPAM) {
        node->keyword = "";
    }
    if (type == TYPE_MONITOR_SPAM) {
        db_insert(node->mail, node->keyword, TYPE_MONITOR, node->port);
        node->keyword = "";
        db_insert(node->mail, node->keyword, TYPE_SPAM, node->port);
    } else {
        db_insert(node->mail, node->keyword, type, node->port);
    }
#endif
    DEBUG("free mail!\n");
    /* 到达这里说明记录了日志 */
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
    return 0;
}
