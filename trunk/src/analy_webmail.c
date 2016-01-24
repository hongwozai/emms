/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：analy_webmail.c
 ** 创建人：路泽亚
 ** 描  述：解析还原模块的webmail部分
 ** 注  意：1.在这里分析得mail结构体（content,attach_name动态分配）
 **          会在日志模块释放
 **
 ** 当前版本： v2.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-18
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-11-30
 ********************************************************************/
#include <nids.h>
#include <zlib.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "inc/log.h"
#include "inc/kill.h"
#include "utils/acq.h"
#include "inc/match.h"
#include "inc/global.h"
#include "utils/debug.h"
#include "utils/mhash.h"
#include "inc/monitor.h"
#include "utils/urlencode.h"
#include "utils/systemlog.h"
#include "inc/analy_webmail.h"

#define TRUE            1
#define FALSE           0
#define WEB_MSG_LEN     10240
#define WEB_HASH_LEN    1024
#define WEB_PACKAGE_LEN 1500

/* chunk解压1500字节的最大长度 */
#define CHUNK_UNCOMPRESS_SIZE 6000

#define KEY_CONTENT_MID 7

#define MSG_T   struct web_msg_node
#define NODE_T  struct web_hash_node

#define ASSERTRET(p)                            \
    do { if (!(p)) return; } while(0);

#define ASSERTRETNUM(p, num)                    \
    do { if (!(p)) return num; } while(0);

#define RETURN(misncomplete, mishead, misreceive)   \
    do {                                            \
        node->isncomplete = (misncomplete);         \
        node->ishead      = (mishead);              \
        node->isreceive   = (misreceive);           \
        return;                                     \
    } while(0);

/* web_msg_node中的type字段 */
enum {
    MSG_DATA,
    MSG_CLOSE
};

/* web_hash_node中的type字段 */
enum {
    NONE,
    SENDMAIL,
    SENDATTACH_1,
    SENDATTACH_2,
    RECEMAIL,
    RECECONTENT,
    RECEATTACH
};

struct web_hash_node {
    HASH_HEAD;
    struct tcp_stream *ts;
    int type;
    int isncomplete;
    int ishead;
    int isfilename;
    int isreceive;
    /* 有chunk时是当前chunk长度，其余是content-length */
    int length;
    /* 内容分析到了多少，对发送邮件就是content，对附件就是附件内容 */
    int size;
    /* 如果内容没有编码就不使用该项，如果内容有编码则该项包括解码的与还未解码的长度 */
    int realsize;
    mail_t *mail;
};

struct web_msg_node {
    struct tcp_stream *ts;
    struct tuple4 t4;
    int type;
    void *data;
    int len;
    int isrequest;
};

static Acq_T     *mq;
static Mhash_T   *hash;
static Mpool_T   *pkgpool;
static Mpool_T   *hnode;
static pthread_t  analy;

static void*analy_callback(void *data);
static void parse(NODE_T *node, void *data, int len, int isrequest);
static int  parse_requestline(char **str, int *len);
static void parse_sendmail(NODE_T *node, void *data, int len);
static int  parse_sendmail_xml(NODE_T *node, void *data, int len, int type);
static void parse_sendattach_1(NODE_T *node, void *data, int len);
static void parse_sendattach_2(NODE_T *node, void *data, int len);
static void parse_recv(NODE_T *node, void *data, int len);
static void parse_recvmail(NODE_T *node, void *data, int len);
static void parse_recvcontent(NODE_T *node, void *data, int len);
static void parse_recvcontent_cg(NODE_T *node, void *data, int len);
static void parse_recvattach(NODE_T *node, void *data, int len);
static int  unz_init(z_stream *strm);
static int  unz_uncompress(z_stream *strm);

int analy_webmail_init(void)
{
    mq = Acq_new(sizeof(MSG_T), WEB_MSG_LEN);
    if (!mq) {
        return -1;
    }
    pkgpool = Mpool_new(WEB_PACKAGE_LEN, POOL_BLOCK);
    if (!pkgpool) {
        Acq_free(mq);
        return -1;
    }
    hnode = Mpool_new(sizeof(NODE_T), POOL_BLOCK);
    if (!hnode) {
        Acq_free(mq);
        Mpool_dispose(pkgpool);
        return -1;
    }
    hash = Mhash_new(WEB_HASH_LEN, sizeof(NODE_T), hnode);
    if (!hash) {
        Acq_free(mq);
        Mpool_dispose(hnode);
        Mpool_dispose(pkgpool);
        return -1;
    }
    return 0;
}

void analy_webmail_stop(void)
{
    int flag;

    if ((flag = pthread_cancel(analy))) {
        ERROR("thread analy can't cancel(%s)!", strerror(flag));
    }
    pthread_join(analy, NULL);

    Acq_free(mq);
    Mhash_free(hash);
    Mpool_dispose(hnode);
    Mpool_dispose(pkgpool);
}

void analy_webmail_run(void)
{
    int flag;

    if ((flag = pthread_create(&analy, NULL, analy_callback, NULL))) {
        ERROR("thread analy can't create!(%s)", strerror(flag));
        return;
    }
}

void analy_webmail_putin(struct tcp_stream *ts)
{
    MSG_T msg;
    struct half_stream *half;
    int state = ts->nids_state;

    assert(ts);
    if (state == NIDS_CLOSE || state == NIDS_RESET) {
        msg.type = MSG_CLOSE;
        msg.t4   = ts->addr;
    } else {
        assert(state == NIDS_DATA);
        msg.type = MSG_DATA;
        msg.ts   = ts;
        if (ts->client.count_new) {
            half = &ts->client;
            msg.isrequest = FALSE;
        } else {
            half = &ts->server;
            msg.isrequest = TRUE;
        }
        msg.len = half->count_new;
        msg.t4  = ts->addr;
        ASSERTRET(msg.data = Mpool_alloc(pkgpool));
        memcpy(msg.data, half->data, msg.len);
    }
    Acq_enq(mq, &msg);
}

/* 判断消息类型，然后做对应的操作 */
static void* analy_callback(void *data)
{
    (void)data;

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    for (;;) {
        MSG_T msg;
        NODE_T *node;

        Acq_deq(mq, &msg);
        if (msg.type == MSG_CLOSE) {
            Mhash_del(hash, &msg.t4);
            continue;
        }
        assert(msg.type == MSG_DATA);
        node = Mhash_find(hash, &msg.t4);
        if (!node) {
            node = Mhash_insert(hash, &msg.ts->addr);
            node->ts = msg.ts;
            node->type        = NONE;
            node->ishead      = FALSE;
            node->isfilename  = FALSE;
            node->isncomplete = FALSE;
            node->isreceive   = FALSE;
        }
        parse(node, msg.data, msg.len, msg.isrequest);
    }
    pthread_exit(NULL);
}

static int parse_requestline(char **str, int *len)
{
    char *p = NULL;
    char *tmp = *str;

    /* tmp + 10 跳过 uri一个字符，空格一个，HTTP/1.1八个 */
    if (tmp[0] == 'P' && tmp[1] == 'O' && tmp[2] == 'S' && tmp[3] == 'T') {
        tmp += 5;
        if (strncmp(tmp, "/upxmail/upload", 15) == 0) {
            return SENDATTACH_1;
        }
        p = strchr(tmp + 10, '\r');
        assert(p);
        *p = '\0';
        p += 2;
        *len -= (p - *str);
        if (strstr(tmp, "upload")) {
            *str = p;
            return SENDATTACH_2;
        }
        if (strstr(tmp, "action=deliver")) {
            *str = p;
            return SENDMAIL;
        }
        if (strstr(tmp, "action=read")) {
            *str = p;
            return RECEMAIL;
        }
    }
    if (tmp[0] == 'G' && tmp[1] == 'E' && tmp[2] == 'T') {
        tmp += 4;
        p = strchr(tmp + 10, '\r');
        assert(p);
        *p = '\0';
        p += 2;
        *len -= (p - *str);
        if (strstr(tmp, "readhtml.jsp")) {
            *str = p;
            return RECECONTENT;
        }
        if (strstr(tmp, "readdata.jsp")) {
            *str = p;
            return RECEATTACH;
        }
    }
    return NONE;
}

static void parse(NODE_T *node, void *data, int len, int isrequest)
{
    int mailtype = NONE;
    char *tmp = data;

    assert(node);
    assert(data);
    assert(len > 0);
    assert(isrequest == TRUE || isrequest == FALSE);
    if (isrequest == TRUE) {
        if (node->isncomplete == FALSE) {
            /* 之所以使用mailtype这个局部变量，是因为如果使用node->type来记录
             * 邮件类型再来后续包时会覆盖，所以采用mailtype记录临时的，
             * 用node->type来记录需要保存的数据*/
            switch((mailtype = parse_requestline((char**)&tmp, &len))) {
            case SENDMAIL:
                node->type = mailtype;
                parse_sendmail(node, tmp, len);
                break;
            case SENDATTACH_1:
                node->type = mailtype;
                parse_sendattach_1(node, tmp, len);
                break;
            case SENDATTACH_2:
                node->type = mailtype;
                parse_sendattach_2(node, tmp, len);
                break;
            case RECEMAIL:
            case RECECONTENT:
            case RECEATTACH:
                node->type = mailtype;
                DEBUG("node->type: %d\n%p\n", node->type, node);
                node->isreceive = TRUE;
                break;
            }
            /* 默认的是丢弃行为 */
            goto free;
        }

        /* node->isncomplete == TRUE */
        switch(node->type) {
        case SENDMAIL: parse_sendmail(node, tmp, len);
            break;
        case SENDATTACH_1: parse_sendattach_1(node, tmp, len);
            break;
        case SENDATTACH_2: parse_sendattach_2(node, tmp, len);
            break;
        default:
            node->isncomplete = FALSE;
            assert(0);
        }
        goto free;
    }

    /* isrequest == FALSE */
    if (node->isreceive == FALSE) {
        goto free;
    }
    /* isreceive == TRUE */
    /* 无论isncomplete是否为TRUE，都是调用这个函数，不用再判断 */
    if (node->type == RECEMAIL || node->type == RECECONTENT) {
        parse_recv(node, tmp, len);
    } else if (node->type == RECEATTACH) {
        parse_recvattach(node, tmp, len);
    } else {
        assert(0);
    }

free:
    Mpool_free(pkgpool, data);
}

static void parse_sendmail(NODE_T *node, void *data, int len)
{
    char *p = data;
    int clength;
    static int flag;

    p[len] = '\0';
    DEBUG("head: %d, isncomplete: %d\ndata: %s\n",
          node->ishead, node->isncomplete, data);
    /* 封堵时要释放 */
    if (node->isncomplete == FALSE) {
        ASSERTRET(node->mail = Mpool_alloc(mailpool));
        MAIL_INIT(node->mail);
        flag = KEY_FROM;
        /* 分析报文 */
        p = strstr(p, "Content-Length: ");
        ASSERTRET(p);
        p += 16;
        strchr_parseint(p, '\r', &node->length);
        /* DEBUG("content-length: %d\n", node->length); */
        if ( !(p = strstr(p, "\r\n\r\n")) ) {
            /* 基本上是这种情况 */
            RETURN(TRUE, TRUE, FALSE);
        }
        p += 4;
        node->length -= len - (int)(p - (char*)data);
        if (node->length <= 0) {
            /* 这个包里有所有的内容，从未遇到这种情况 */
            assert(0);
            return;
        }
        /* 这个包中有部分内容，限制到1500之后也没有遇到这种情况 */
        assert(0);
        return;
    }

    /* node->isncomplete == TRUE */
    if (node->ishead == TRUE) {
        if ( !(p = strstr(p, "\r\n\r\n")) ) {
            RETURN(TRUE, TRUE, FALSE);
        }
        /* 这个包中有实体 */
        p += 4;
        clength = len - (int)(p - (char*)data);
        node->length -= clength;
        flag = parse_sendmail_xml(node, p, clength, KEY_FROM);
        if (flag == -2) {
            /* 内存出错 */
            RETURN(FALSE, FALSE, FALSE);
        }
        if (flag == -1) {
            /* 封堵成功 */
            RETURN(FALSE, FALSE, FALSE);
        }
        if (flag <= KEY_CONTENT_MID) {
            /* 没分析完 */
            RETURN(TRUE, FALSE, FALSE);
        }
        if (node->length <= 0) {
            /* 这个http报文完结了，需要监测 */
            monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
            RETURN(FALSE, FALSE, FALSE);
        }
        monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
        RETURN(FALSE, FALSE, FALSE);
    }

    /* node->ishead == FALSE */
    /* 数据包只包含实体 */
    node->length -= len;
    if (node->length <= 0) {
        flag = parse_sendmail_xml(node, p, len, flag);
        if (flag != -1 && flag != -2) {
            monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
        }
        RETURN(FALSE, FALSE, FALSE);
    }
    /* 还有下一个数据包 */
    flag = parse_sendmail_xml(node, p, len, flag);
    if (flag == -1) {
        RETURN(FALSE, FALSE, FALSE);
    }
    if (flag == -2) {
        /* 内存错误 */
        RETURN(FALSE, FALSE, FALSE);
    }
    if (flag <= KEY_CONTENT_MID) {
        /* 后面的包还有content的一部分没有分析完 */
        RETURN(TRUE, FALSE, FALSE);
    }
    DEBUG("content: %s\n", node->mail->content);
    monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
    RETURN(FALSE, FALSE, FALSE);
}

/* type 代表从那个关键字开始分析,
 * 封堵完毕时返回负数
 */
static int parse_sendmail_xml(NODE_T *node, void *data, int len, int type)
{
    int tmplen, newsize;
    char *p = data, *q, *tmp;

    switch(type) {
    case KEY_FROM:
        ASSERTRETNUM(p = strstr(p, "account%22%3E"), KEY_FROM);
        p += 13;
        ASSERTRETNUM(q = strstr(p, "%3C%2F"), KEY_TO);
        url_decode(p, (int)(q - p), node->mail->from);
        DEBUG("from: %s\n", node->mail->from);
        tmp = match_match(match_use[TYPE_KILL][KEY_FROM], node->mail->from);
        if (tmp) {
            kill_tcp_request(node->ts);
            DEBUG("account: %s\nkeyword: %s\n", node->mail->from, tmp);
            log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
            return -1;
        }
        p = q;

    case KEY_TO:
        ASSERTRETNUM(p = strstr(p, "to%22%3E%3Cstring%3E"), KEY_TO);
        p += 20;
        ASSERTRETNUM(q = strstr(p, "%3C%2Fstring%3E%3C%2Farray"), KEY_SUBJECT);
        url_decode(p, (int)(q - p), node->mail->to);
        DEBUG("to: %s\n", node->mail->to);
        tmp = match_match(match_use[TYPE_KILL][KEY_TO], node->mail->to);
        if (tmp) {
            kill_tcp_request(node->ts);
            DEBUG("to: %s\nkeyword: %s\n", node->mail->to, tmp);
            log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
            return -1;
        }
        p = q;

    case KEY_SUBJECT:
        ASSERTRETNUM(p = strstr(p, "subject%22%3E"), KEY_SUBJECT);
        p += 13;
        ASSERTRETNUM(q = strstr(p, "%3C%2Fstring"), KEY_CONTENT);
        url_decode(p, (int)(q - p), node->mail->subject);
        DEBUG("subject: %s\n", node->mail->subject);
        tmp = match_match(match_use[TYPE_KILL][KEY_SUBJECT],
                          node->mail->subject);
        if (tmp) {
            kill_tcp_request(node->ts);
            DEBUG("subject: %s\nkeyword: %s\n", node->mail->subject, tmp);
            log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
            return -1;
        }
        p = q;

    case KEY_CONTENT:
        ASSERTRETNUM(p = strstr(p, "content%22%3E"), KEY_CONTENT);
        p += 13;

        /* 如果content的内容全在这个包里 */
        if ((q = strstr(p, "%3C%2Fstring"))) {
            node->size = url_decode_replace(p, (int)(q - p));
        } else {
            /* content没有完，但是到了包边界 */
            tmplen = len - (int)(p - (char*)data);
            /* FIXME:这里有个url解码的问题 */
            node->size = url_decode_replace(p, tmplen);
        }

        tmp = match_match(match_use[TYPE_KILL][KEY_CONTENT], p);
        if (tmp) {
            kill_tcp_request(node->ts);
        }
        ASSERTRETNUM(ALLOC(node->mail->content, node->size + 1), -2);
        memcpy(node->mail->content, p, node->size);
        node->mail->content[node->size] = '\0';
        if (tmp) {
            DEBUG("content: %s\nkeyword: %s\n", node->mail->content, tmp);
            log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
            return -1;
        }
        if (q) {
            DEBUG("content: %s\n", node->mail->content);
            return KEY_CONTENT_MID + 1;
        }
        return KEY_CONTENT_MID;

    case KEY_CONTENT_MID:
        /* 在这个包里content完结 */
        if ((q = strstr(p, "%3C%2Fstring"))) {
            newsize = url_decode_replace(p, (int)(q - p));
            tmp = match_match(match_use[TYPE_KILL][KEY_CONTENT], p);
            if (tmp) {
                kill_tcp_request(node->ts);
                ASSERTRETNUM(REALLOC(node->mail->content, node->mail->content,
                                     node->size + newsize + 1), -2);
                memcpy(node->mail->content + node->size, p, newsize);
                node->mail->content[node->size + newsize] = '\0';
                DEBUG("content: %s\nkeyword: %s\n", node->mail->content, tmp);
                log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
                return -1;
            }
            ASSERTRETNUM(REALLOC(node->mail->content, node->mail->content,
                                 node->size + newsize + 1), -2);
            memcpy(node->mail->content + node->size, p, newsize);
            node->mail->content[node->size + newsize] = '\0';
            DEBUG("content: %s\n", node->mail->content);
            return KEY_CONTENT_MID + 1;
        }
        /* 这个包里content还没有完结 */
        newsize = url_decode_replace(p, len);
        tmp = match_match(match_use[TYPE_KILL][KEY_CONTENT], p);
        if (tmp) {
            kill_tcp_request(node->ts);
        }
        ASSERTRETNUM(REALLOC(node->mail->content, node->mail->content,
                             node->size + newsize + 1), -2);
        memcpy(node->mail->content + node->size, p, newsize);
        node->mail->content[node->size + newsize] = '\0';
        if (tmp) {
            DEBUG("content: %s\nkeyword: %s\n", node->mail->content, tmp);
            log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
            return -1;
        }
        node->size += newsize;
        return KEY_CONTENT_MID;

    default:
        assert(0);
        return -1;
    }
}

/* /upxmail/upload形式，附件名在http的mime中 */
/* Mail-Upload-name, Mail-Upload-length, Content-type */
/* 内容无格式 */
static void parse_sendattach_1(NODE_T *node, void *data, int len)
{
    char *p = data, *tmp;
    char filename[128];

    p[len] = '\0';
    if (node->isncomplete == FALSE) {
        ASSERTRET(node->mail = Mpool_alloc(mailpool));
        MAIL_INIT(node->mail);

        /* 分析 */
        p = strstr(p, "Mail-Upload-name: ");
        /* p = strcpychr(p + 18, node->mail->attach_name, '\r'); */
        p = strcpychr(p + 18, filename, '\r');
        DEBUG("filename: %s\n", filename);
        str_decode(filename, node->mail->attach_name);
        tmp = match_match(match_use[TYPE_KILL][KEY_ATTACH_NAME],
                          node->mail->attach_name);
        if (tmp) {
            kill_tcp_response(node->ts);
            DEBUG("killtcp!, attach_name: %s\nkeyword: %s\n",
                  node->mail->attach_name, tmp);
            log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
            RETURN(FALSE, FALSE, FALSE);
        }
        /* 获得长度 */
        p = strstr(p, "Mail-Upload-length: ");
        p = strchr_parseint(p + 20, '\r', &node->length);
        DEBUG("filename: %s, length: %d\n", node->mail->attach_name,
              node->length);

        if (!istxt(node->mail->attach_name)) {
            /* 放入监测队列 */
            monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
            RETURN(FALSE, FALSE, FALSE);
        }
        /* 是文本文件 */
        if (!(p = strstr(p, "\r\n\r\n"))) {
            /* 未出现此种情况 */
            assert(0);
        }
        p += 4;
        node->size = len - (int)(p - (char*)data);
        node->length -= node->size;
        /* 所有附件内容都在这里 */
        tmp = match_match(match_use[TYPE_KILL][KEY_ATTACH_CONTENT], p);
        if (tmp) {
            kill_tcp_response(node->ts);
            if (!ALLOC(node->mail->attach_content, node->length + 1)) {
                RETURN(FALSE, FALSE, FALSE);
            }
            memcpy(node->mail->attach_content, p, node->size);
            node->mail->attach_content[node->size] = '\0';
            DEBUG("killtcp!, attach_content: %s\nkeyword: %s\n", p, tmp);
            /* 放入日志队列 */
            log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
            RETURN(FALSE, FALSE, FALSE);
        }
        if (!ALLOC(node->mail->attach_content, node->size + 1)) {
            RETURN(FALSE, FALSE, FALSE);
        }
        memcpy(node->mail->attach_content, p, node->size);
        node->mail->attach_content[node->size] = '\0';
        if (node->length <= 0) {
            /* 放入监测队列 */
            monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
            RETURN(FALSE, FALSE, FALSE);
        }
        RETURN(TRUE, FALSE, FALSE);
    }

    /* 实体数据部分 */
    node->length -= len;
    /* 数据到此结束 */
    tmp = match_match(match_use[TYPE_KILL][KEY_ATTACH_CONTENT], p);
    if (tmp) {
        kill_tcp_response(node->ts);
        if (!REALLOC(node->mail->attach_content, node->mail->attach_content,
                     node->size + len + 1)) {
            RETURN(FALSE, FALSE, FALSE);
        }
        memcpy(node->mail->attach_content + node->size, p, len);
        node->mail->attach_content[node->size + len] = '\0';
        DEBUG("killtcp!, attach_content: %s\nkeyword: %s\n",
              node->mail->attach_content, tmp);
        /* 放入日志队列 */
        log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
        RETURN(FALSE, FALSE, FALSE);
    }
    if (!REALLOC(node->mail->attach_content, node->mail->attach_content,
                 node->size + len + 1)) {
        RETURN(FALSE, FALSE, FALSE);
    }
    memcpy(node->mail->attach_content + node->size, p, len);
    node->mail->attach_content[node->size + len] = '\0';
    if (node->length <= 0) {
        /* 放入监测队列 */
        monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
        DEBUG("content: %s\n", node->mail->attach_content);
        RETURN(FALSE, FALSE, FALSE);
    }
    node->size += len;
    RETURN(TRUE, FALSE, FALSE);
}

/**
 * 由于只有一个boundary，所以直接忽视该boundary，直接查找文件名
 */
static void parse_sendattach_2(NODE_T *node, void *data, int len)
{
    char filename[128];
    char *p = data, *tmp, *head;

    p[len] = '\0';
    DEBUG("p: %s\n", p);
    if (node->isncomplete == FALSE) {
        ASSERTRET(node->mail = Mpool_alloc(mailpool));
        MAIL_INIT(node->mail);

        /* 分析 */
        p = strstr(data, "Content-Length: ");
        strchr_parseint(p + 16, '\r', &node->length);
        if (!(p = strstr(p, "\r\n\r\n"))) {
            RETURN(TRUE, TRUE, FALSE);
        }
        /* 未出现头部在第一个包中完结的情况 */
        assert(0);
    }

    /* node->isncomplete == TRUE */
    if (node->ishead == TRUE) {
        /* 继续找头部，计算长度 */
        if (!(p = strstr(p, "\r\n\r\n"))) {
            RETURN(TRUE, TRUE, FALSE);
        }
        p += 4;
        head = p;
        /* 假设filename与\r\n\r\n在一个包中 */
        if (!(p = strstr(p, "filename="))) {
            node->isfilename = TRUE;
            RETURN(TRUE, FALSE, FALSE);
        }
        p = strcpychr(p + 10, filename, '"');
        DEBUG("filename: %s\n", filename);
        str_decode(filename, node->mail->attach_name);
        /* p = strcpychr(p + 10, node->mail->attach_name, '"'); */
        /* url_decode_replace(node->mail->attach_name, */
        /*                    strlen(node->mail->attach_name)); */
        tmp = match_match(match_use[TYPE_KILL][KEY_ATTACH_NAME],
                          node->mail->attach_name);
        if (tmp) {
            kill_tcp_response(node->ts);
            DEBUG("attach_name: %s\nkeyword: %s\n", node->mail->attach_name,
                  tmp);
            /* 放入日志队列 */
            log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
            RETURN(FALSE, FALSE, FALSE);
        }
        DEBUG("attach_name: %s\n", node->mail->attach_name);
        /* 封堵完成再判断是否是txt */
        if (!istxt(node->mail->attach_name)) {
            monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
            RETURN(FALSE, FALSE, FALSE);
        }

        /* 是txt文件，继续分析，跳过\r\nContent-Type: text/plain\r\n\r\n */
        p += 30;
        node->size = len - (int)(p - head);
        node->length -= node->size;
        /* 之后的是数据 */
        tmp = match_match(match_use[TYPE_KILL][KEY_ATTACH_CONTENT], p);
        if (tmp) {
            kill_tcp_response(node->ts);
        }
        if (!ALLOC(node->mail->attach_content, node->size + 1)) {
            RETURN(FALSE, FALSE, FALSE);
        }
        memcpy(node->mail->attach_content, p, node->size);
        node->mail->attach_content[node->size] = '\0';
        if (tmp) {
            DEBUG("killtcp! attachcontent: %s\nkeyword: %s\n",
                  node->mail->attach_content, tmp);
            /* 放入封堵日志队列 */
            log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
            RETURN(FALSE, FALSE, FALSE);
        }
        if (node->length <= 0) {
            DEBUG("content: %s\n", node->mail->attach_content);
            monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
            RETURN(FALSE, FALSE, FALSE);
        }
        RETURN(TRUE, FALSE, FALSE);
    }

    if (node->isfilename == TRUE) {
        /* 假设filename与\r\n\r\n在一个包中 */
        if (!(p = strstr(p, "filename="))) {
            node->isfilename = TRUE;
            RETURN(TRUE, FALSE, FALSE);
        }
        p = strcpychr(p + 10, filename, '"');
        DEBUG("filename: %s\n", filename);
        str_decode(filename, node->mail->attach_name);
        /* p = strcpychr(p + 10, node->mail->attach_name, '"'); */
        /* url_decode_replace(node->mail->attach_name, */
        /*                    strlen(node->mail->attach_name)); */
        tmp = match_match(match_use[TYPE_KILL][KEY_ATTACH_NAME],
                          node->mail->attach_name);
        if (tmp) {
            kill_tcp_response(node->ts);
            DEBUG("attach_name: %s\nkeyword: %s\n", node->mail->attach_name,
                  tmp);
            /* 放入日志队列 */
            log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
            RETURN(FALSE, FALSE, FALSE);
        }
        DEBUG("attach_name: %s\n", node->mail->attach_name);
        /* 封堵完成再判断是否是txt */
        if (!istxt(node->mail->attach_name)) {
            monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
            RETURN(FALSE, FALSE, FALSE);
        }

        /* 是txt文件，继续分析，跳过\r\nContent-Type: text/plain\r\n\r\n */
        p += 30;
        node->size = len - (int)(p - (char*)data);
        node->length -= node->size;
        /* 之后的是数据 */
        tmp = match_match(match_use[TYPE_KILL][KEY_ATTACH_CONTENT], p);
        if (tmp) {
            kill_tcp_response(node->ts);
        }
        if (!ALLOC(node->mail->attach_content, node->size + 1)) {
            RETURN(FALSE, FALSE, FALSE);
        }
        memcpy(node->mail->attach_content, p, node->size);
        node->mail->attach_content[node->size] = '\0';
        if (tmp) {
            DEBUG("killtcp! attachcontent: %s\nkeyword: %s\n",
                  node->mail->attach_content, tmp);
            /* 放入封堵日志队列 */
            log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
            RETURN(FALSE, FALSE, FALSE);
        }
        if (node->length <= 0) {
            DEBUG("content: %s\n", node->mail->attach_content);
            monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
            RETURN(FALSE, FALSE, FALSE);
        }
        RETURN(TRUE, FALSE, FALSE);
    }

    /* node->ishead == FALSE, node->isfilename == FALSE */
    node->length -= len;
    tmp = match_match(match_use[TYPE_KILL][KEY_ATTACH_CONTENT], p);
    if (tmp) {
        kill_tcp_response(node->ts);
    }
    if (!REALLOC(node->mail->attach_content, node->mail->attach_content,
                 node->size + len + 1)) {
        RETURN(FALSE, FALSE, FALSE);
    }
    memcpy(node->mail->attach_content + node->size, p, len);
    node->mail->attach_content[node->size + len] = '\0';
    if (tmp) {
        DEBUG("killtcp! attachcontent: %s\nkeyword: %s\n",
              node->mail->attach_content, tmp);
        /* 放入封堵日志队列 */
        log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
        RETURN(FALSE, FALSE, FALSE);
    }
    if (node->length <= 0) {
        DEBUG("content: %s\n", node->mail->attach_content);
        /* 放入监测队列 */
        monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
        RETURN(FALSE, FALSE, FALSE);
    }
    RETURN(TRUE, FALSE, FALSE);
}

static void parse_recv(NODE_T *node, void *data, int len)
{
    char *p = data, *q;
    static int gzip = 1;

    p[len] = '\0';
    if (node->isncomplete == FALSE) {
        gzip = 1;

        if ((q = strstr(p, "Content-Length: "))) {
            gzip = 0;
            q = strchr_parseint(q + 16, '\r', &node->length);
        }
        p = strstr(p, "\r\n\r\n");
        ASSERTRET(p);
        p += 4;
        len -= (int)(p - (char*)data);
        if (node->type == RECEMAIL) {
            parse_recvmail(node, p, len);
        } else {
            /* node->type == RECECONTENT */
            if (gzip == 0) {
                parse_recvcontent(node, p, len);
            } else {
                parse_recvcontent_cg(node, p, len);
            }
        }
        return;
    }

    /* node->isncomplete == TRUE */
    if (node->type == RECEMAIL) {
        parse_recvmail(node, p, len);
    } else {
        /* node->type == RECECONTENT */
        if (gzip == 0) {
            parse_recvcontent(node, p, len);
        } else {
            parse_recvcontent_cg(node, p, len);
        }
    }
}

/* json */
/* 这个情况只有一个包 */
static void parse_recvmail(NODE_T *node, void *data, int len)
{
    int ret;
    char *p = data, *tmp;
    z_stream strm;
    char buf[CHUNK_UNCOMPRESS_SIZE];

    if (node->isncomplete == TRUE) {
        assert(0);
        RETURN(FALSE, FALSE, FALSE);
    }
    ASSERTRET(node->mail = Mpool_alloc(mailpool));
    MAIL_INIT(node->mail);

    ret = unz_init(&strm);
    if (ret == -1) goto ret;
    p = strchr_parse16(p, '\r', &node->length);
    DEBUG("length: %d\n", node->length);
    p += 2;

    /* FIXME:只分析一个包中的内容 */
    strm.avail_in  = len;
    strm.next_in   = (unsigned char*)p;
    strm.avail_out = CHUNK_UNCOMPRESS_SIZE;
    strm.next_out  = (unsigned char*)buf;
    ret = unz_uncompress(&strm);
    if (ret == -1) goto ret;
    buf[CHUNK_UNCOMPRESS_SIZE - strm.avail_out] = '\0';
    DEBUG("recvmail: %s\n", buf);
    p = strstr(buf, "'from':");
    /* 跳过'from':[' */
    p += 9;
    p = strcpychr(p, node->mail->from, '\'');
    tmp = match_match(match_use[TYPE_KILL][KEY_FROM], node->mail->from);
    if (tmp) {
        kill_tcp_response(node->ts);
        log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
        DEBUG("from: %s\nkeyword: %s\n", node->mail->from, tmp);
        RETURN(FALSE, FALSE, FALSE);
    }
    /* 跳过],\n'to':['*/
    p += 11;
    p = strcpychr(p, node->mail->to, '\'');
    tmp = match_match(match_use[TYPE_KILL][KEY_TO], node->mail->to);
    if (tmp) {
        kill_tcp_response(node->ts);
        log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
        DEBUG("to: %s\nkeyword: %s\n", node->mail->to, tmp);
        RETURN(FALSE, FALSE, FALSE);
    }
    p = strstr(p, "'subject':");
    p += 11;
    /* 跳过'subject':' */
    p = strcpychr(p, node->mail->subject, '\'');
    tmp = match_match(match_use[TYPE_KILL][KEY_SUBJECT], node->mail->subject);
    if (tmp) {
        kill_tcp_response(node->ts);
        log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
        DEBUG("subject: %s\nkeyword: %s\n", node->mail->subject, tmp);
        RETURN(FALSE, FALSE, FALSE);
    }
    while ((p = strstr(p, "'filename':"))) {
        p += 11;
        strcpychr(p, node->mail->attach_name, '\n');
        tmp = match_match(match_use[TYPE_KILL][KEY_ATTACH_NAME],
                          node->mail->attach_name);
        if (tmp) {
            kill_tcp_response(node->ts);
            log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
            DEBUG("attach_name: %s\nkeyword: %s\n", node->mail->attach_name,
                  tmp);
            RETURN(FALSE, FALSE, FALSE);
        }
    }
    monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
    inflateEnd(&strm);
ret:
    /* 之后的包不需要了 */
    RETURN(FALSE, FALSE, FALSE);
}

/* html */
/* 可能有chunk+gzip也可能什么都没有 */
/* 没有gzip的用node->length标识长度 */
static void parse_recvcontent(NODE_T *node, void *data, int len)
{
    char *p = data, *tmp;
    static int size;

    DEBUG("data: %s\n", data);
    if (node->isncomplete == FALSE) {
        ASSERTRET(node->mail = Mpool_alloc(mailpool));
        MAIL_INIT(node->mail);
        size = 0;

        node->length -= len;
        tmp = match_match(match_use[TYPE_KILL][KEY_CONTENT], p);
        if (tmp) {
            kill_tcp_response(node->ts);
        }
        if (!ALLOC(node->mail->content, len + 1))
            RETURN(FALSE, FALSE, FALSE);
        memcpy(node->mail->content, p, len);
        size += len;
        node->mail->content[size] = '\0';
        if (tmp) {
            log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
            RETURN(FALSE, FALSE, FALSE);
        }
        if (node->length <= 0) {
            monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
            DEBUG("content: %s\n", node->mail->content);
            RETURN(FALSE, FALSE, FALSE);
        }
        RETURN(TRUE, FALSE, TRUE);
    }

    /* node->isncomplete == TRUE */
    node->length -= len;
    tmp = match_match(match_use[TYPE_KILL][KEY_CONTENT], p);
    if (tmp) {
        kill_tcp_response(node->ts);
    }
    if (!REALLOC(node->mail->content, node->mail->content, len + 1))
        RETURN(FALSE, FALSE, FALSE);
    memcpy(node->mail->content + size, p, len);
    size += len;
    node->mail->content[len] = '\0';
    if (tmp) {
        log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
        RETURN(FALSE, FALSE, FALSE);
    }
    if (node->length <= 0) {
        monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
        DEBUG("content: %s\n", node->mail->content);
        RETURN(FALSE, FALSE, FALSE);
    }
    RETURN(TRUE, FALSE, TRUE);
}

/* chunk + gzip */
/* chunk情况下一定只有一个chunk，但是可能分多个包，可能有gzip
 * zlib中间next_in, avail_in需要记录下来
 */
static void parse_recvcontent_cg(NODE_T *node, void *data, int len)
{
    int ret;
    char *p = data, *tmp;
    char buf[CHUNK_UNCOMPRESS_SIZE];
    /* 解压之后的数据长度 */
    int ripesize;
    /* 内容累计的长度 */
    static int contentlen = 0;
    static z_stream strm;
    /* 解压之前的长度 */
    static int  rawsize;
    static char rawbuf[CHUNK_UNCOMPRESS_SIZE];
    char **addr = NULL;

    if (node->isncomplete == FALSE) {
        ASSERTRET(node->mail = Mpool_alloc(mailpool));
        MAIL_INIT(node->mail);
        if (node->type == RECECONTENT) {
            addr = &node->mail->content;
        } else if (node->type == RECEATTACH) {
            addr = &node->mail->attach_content;
        } else {
            assert(0);
        }
        rawsize    = 0;
        contentlen = 0;
        ret = unz_init(&strm);
        if (ret == -1) goto ret;

        p = strchr_parse16(p, '\r', &node->length);
        DEBUG("length: %d\n", node->length);
        p += 2;
        len -= (int)(p - (char*)data);

        node->length  -= len;
        DEBUG("len: %d\nnode->length: %d\n", len, node->length);
        if (node->length == -7) {
            strm.avail_in = len - 7;
        } else {
            strm.avail_in = len;
        }
        strm.next_in   = (unsigned char*)p;
        strm.avail_out = CHUNK_UNCOMPRESS_SIZE;
        strm.next_out  = (unsigned char*)buf;
        ret = unz_uncompress(&strm);
        if (ret == -1) goto ret;
        ripesize = CHUNK_UNCOMPRESS_SIZE - strm.avail_out;
        buf[ripesize] = '\0';
        DEBUG("buf: %s\n", buf);

        if (node->type == RECECONTENT) {
            tmp = match_match(match_use[TYPE_KILL][KEY_CONTENT], buf);
        } else {
            tmp = match_match(match_use[TYPE_KILL][KEY_ATTACH_CONTENT], buf);
        }
        if (tmp) {
            kill_tcp_response(node->ts);
        }
        if (!ALLOC(*addr, ripesize)) {
            goto ret;
        }
        memcpy(*addr, buf, ripesize);
        contentlen += ripesize;
        (*addr)[contentlen] = '\0';
        if (tmp) {
            log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
            RETURN(FALSE, FALSE, FALSE);
        }

        /* 可能有7个字节\r\n0\r\n\r\n */
        if (strm.avail_in != 0) {
            memcpy(rawbuf, strm.next_in, strm.avail_in);
            rawsize = strm.avail_in;
        } else {
            rawsize = 0;
        }

        if (node->length <= 0) {
            /* 仅有一个包 */
            inflateEnd(&strm);
            goto ret;
        }
        /* 还有后续 */
        RETURN(TRUE, FALSE, TRUE);
    ret:
        monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
        RETURN(FALSE, FALSE, FALSE);
    }

    if (node->type == RECECONTENT) {
        addr = &node->mail->content;
    } else if (node->type == RECEATTACH) {
        addr = &node->mail->attach_content;
    } else {
        assert(0);
    }
    /* 这里是后续包的分析 */
    /* node->isncomplete == TRUE */
    node->length  -= len;
    /* 如果没有之前未解压的数据，那么就不必复制整个包 */
    if (rawsize != 0) {
        memcpy(rawbuf + rawsize, data, len);
        strm.avail_in  = rawsize + len;
        strm.next_in   = (unsigned char*)rawbuf;
        rawsize = 0;
    } else {
        strm.avail_in  = len;
        strm.next_in   = (unsigned char*)data;
    }
    strm.avail_out = CHUNK_UNCOMPRESS_SIZE;
    strm.next_out  = (unsigned char*)buf;
    ret = unz_uncompress(&strm);
    if (ret == -1) goto ret;
    ripesize = CHUNK_UNCOMPRESS_SIZE - strm.avail_out;
    buf[ripesize] = '\0';
    DEBUG("buf: %s\n", buf);

    if (node->type == RECECONTENT) {
        tmp = match_match(match_use[TYPE_KILL][KEY_CONTENT], buf);
    } else {
        tmp = match_match(match_use[TYPE_KILL][KEY_ATTACH_CONTENT], buf);
    }
    if (tmp) {
        kill_tcp_response(node->ts);
    }
    if (!REALLOC(*addr, *addr, contentlen + ripesize)) {
        goto ret2;
    }
    memcpy(*addr + contentlen, buf, ripesize);
    contentlen += ripesize;
    (*addr)[contentlen] = '\0';
    if (tmp) {
        log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
        inflateEnd(&strm);
        RETURN(FALSE, FALSE, FALSE);
    }
    if (strm.avail_in != 0) {
        memcpy(rawbuf, strm.next_in, strm.avail_in);
        rawsize = strm.avail_in;
    } else {
        rawsize = 0;
    }

    if (node->length <= 0) {
        goto ret2;
    }
    RETURN(TRUE, FALSE, TRUE);

ret2:
    monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
    DEBUG("content: %s\n", *addr);
    inflateEnd(&strm);
    RETURN(FALSE, FALSE, FALSE);
}

/* 可能有gzip+chunk的，也可能没有gzip仅有chunk */
/* 实体部分chunk加gzip与recvattach一模一样 */
static void parse_recvattach(NODE_T *node, void *data, int len)
{
    static int isgzip = 0;
    static int size = 0;
    char filename[128];
    char *p = data, *tmp, *head;

    p[len] = '\0';
    if (node->isncomplete == FALSE) {
        ASSERTRET(node->mail = Mpool_alloc(mailpool));
        MAIL_INIT(node->mail);
        size = 0;

        /* 获取filename */
        p = strstr(p, "filename=\"");
        p += 10;
        p = strcpychr(p, filename, '\"');
        str_decode(filename, node->mail->attach_name);
        DEBUG("attachname: %s\n", node->mail->attach_name);

        tmp = match_match(match_use[TYPE_KILL][KEY_ATTACH_NAME],
                          node->mail->attach_name);
        if (tmp) {
            kill_tcp_response(node->ts);
            DEBUG("attach_name: %s\nkeyword: %s\n", node->mail->attach_name,
                  tmp);
            log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
            RETURN(FALSE, FALSE, FALSE);
        }

        if (!istxt(node->mail->attach_name)) {
            /* 放入监测队列 */
            monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
            RETURN(FALSE, FALSE, FALSE);
        }
        /* 是文本文件，默认gzip在filename后面 */
        /* TODO:这里可以加快速度 */
        if (strstr(p, "gzip")) {
            isgzip = 1;
        } else {
            isgzip = 0;
        }

        p = strstr(p, "\r\n\r\n");
        p += 4;
        head = p;
        len -= (int)(p - (char*)data);
        if (isgzip == 1) {
            parse_recvcontent_cg(node, p, len);
            return;
        }
        /* 不是gzip的情况，chunk */
        p = strchr_parse16(p, '\r', &node->length);
        DEBUG("length: %d\n", node->length);
        p += 2;
        len -= (int)(p - head);
        node->length -= len;
        DEBUG("p: %s\n", p);
        tmp = match_match(match_use[TYPE_KILL][KEY_ATTACH_CONTENT], p);
        if (tmp) {
            kill_tcp_response(node->ts);
        }
        if (!ALLOC(node->mail->attach_content, len + 1)) {
            RETURN(FALSE, FALSE, FALSE);
        }
        memcpy(node->mail->attach_content, p, len);
        size += len;
        node->mail->attach_content[size] = '\0';
        if (tmp) {
            DEBUG("attach_content: %s\nkeyword: %s\n",
                  node->mail->attach_content, tmp);
            log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
            RETURN(FALSE, FALSE, FALSE);
        }
        if (node->length <= 0) {
            DEBUG("attach_content: %s\n", node->mail->attach_content);
            monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
            RETURN(FALSE, FALSE, FALSE);
        }
        RETURN(TRUE, FALSE, TRUE);
    }

    /* node->isncomplete == TRUE */
    if (isgzip == 1) {
        parse_recvcontent_cg(node, data, len);
        return;
    }
    /* 不是gzip的情况 */
    node->length -= len;
    tmp = match_match(match_use[TYPE_KILL][KEY_ATTACH_CONTENT], p);
    if (tmp) {
        kill_tcp_response(node->ts);
    }
    if (!REALLOC(node->mail->attach_content, node->mail->attach_content,
                 size + len + 1)) {
        RETURN(FALSE, FALSE, FALSE);
    }
    memcpy(node->mail->attach_content + size, p, len);
    size += len;
    node->mail->attach_content[size] = '\0';
    if (tmp) {
        DEBUG("attach_content: %s\nkeyword: %s\n",
              node->mail->attach_content, tmp);
        log_push(log_queue[PROTO_WEB][TYPE_KILL], node->mail, tmp, WEBMAIL);
        RETURN(FALSE, FALSE, FALSE);
    }
    if (node->length <= 0) {
        DEBUG("attach_content: %s\n", node->mail->attach_content);
        monitor_push(monitor_queue[PROTO_WEB], node->mail, WEBMAIL);
        RETURN(FALSE, FALSE, FALSE);
    }
    RETURN(TRUE, FALSE, TRUE);
}

static int unz_init(z_stream *strm)
{
    int ret;

    strm->zalloc = Z_NULL;
    strm->zfree  = Z_NULL;
    strm->opaque = Z_NULL;
    strm->avail_in = 0;
    strm->next_in = Z_NULL;
    ret = inflateInit2(strm, 47);
    if (ret != Z_OK) {
        DEBUG("inflate init error\n");
        return -1;
    }
    return 0;
}

static int unz_uncompress(z_stream *strm)
{
    int ret;

    ret = inflate(strm, Z_NO_FLUSH);
    switch(ret) {
    case Z_NEED_DICT:
        DEBUG("need dict\n");
        goto error;
    case Z_DATA_ERROR:
        DEBUG("inflate data error\n");
        goto error;
    case Z_MEM_ERROR:
        DEBUG("inflate mem error!\n");
        goto error;
    }
    return 0;
error:
    return -1;
}
