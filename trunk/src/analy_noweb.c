/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：analy_noweb.c
 ** 创建人：路泽亚
 ** 描  述：非webmail部分
 ** 注  意：1.只支持一个附件
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-27
 ********************************************************************/
#include <nids.h>
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
#include "utils/base64.h"
#include "utils/urlencode.h"
#include "utils/systemlog.h"
#include "inc/analy_noweb.h"

#define TRUE              1
#define FALSE             0
#define NOWEB_MSG_LEN     10240
#define NOWEB_HASH_LEN    1024
#define NOWEB_PACKAGE_LEN 1500

#define MSG_T   struct noweb_msg_node
#define NODE_T  struct noweb_hash_node

#define ASSERTRET(p)                            \
    do { if (!(p)) return; } while(0);

#define ASSERTRETLABEL(p, label)                      \
    do { if (!(p)) goto label; } while(0);

#define ASSERTRETNUM(p, num)                    \
    do { if (!(p)) return num; } while(0);

#define TRANS(_state)                               \
    do {state = (_state); goto start;} while(0);

/* noweb_msg_node中的type字段 */
enum {
    MSG_DATA,
    MSG_CLOSE
};

enum {
    CONTENT_TEXT,
    CONTENT_ALTERNATIVE,
    CONTENT_MIXED
};

enum {
    NONE = 0,
    BASE64,
    QUOTED
};

enum {
    STATE_START = 0,
    STATE_NOCONTENT,
    STATE_CONTENTMID,
    STATE_ATTACH,
    STATE_ATTACHMID
};

struct noweb_hash_node {
    HASH_HEAD;
    struct tcp_stream *ts;
    /* 使用define定义的端口号，用端口号区分协议 */
    int port;
    int isncomplete;
    int length;
    int type;
    mail_t *mail;
};

struct noweb_msg_node {
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
static int  parse_mime(NODE_T *node, void *data, int len);
static int  parse_content(NODE_T *node, void *data, int len, int state);
static int  contenttype(char *p);
static int  transfer(char *p);

int analy_noweb_init(void)
{
    mq = Acq_new(sizeof(MSG_T), NOWEB_MSG_LEN);
    if (!mq) {
        return -1;
    }
    pkgpool = Mpool_new(NOWEB_PACKAGE_LEN, POOL_BLOCK);
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
    hash = Mhash_new(NOWEB_HASH_LEN, sizeof(NODE_T), hnode);
    if (!hash) {
        Acq_free(mq);
        Mpool_dispose(hnode);
        Mpool_dispose(pkgpool);
        return -1;
    }
    return 0;
}

void analy_noweb_stop(void)
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

void analy_noweb_run(void)
{
    int flag;

    if ((flag = pthread_create(&analy, NULL, analy_callback, NULL))) {
        ERROR("thread analy can't create!(%s)", strerror(flag));
        return;
    }
}

void analy_noweb_putin(struct tcp_stream *ts)
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
        /* 预处理的地方 */
        if (ts->addr.dest == SMTP && ts->client.count_new) {
            /* 如果是smtp且是响应，那就直接丢掉 */
            return;
        }
        if (ts->addr.dest == IMAP && ts->server.count_new) {
            /* 如果是imap且是请求，那就直接丢掉 */
            return;
        }
        if (ts->addr.dest == POP3 && ts->server.count_new) {
            /* 如果是pop3且是请求，那就直接丢掉 */
            return;
        }
        /* 放入队列中 */
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
            node->ts          = msg.ts;
            node->port        = msg.ts->addr.dest;
            node->mail        = NULL;
            node->isncomplete = FALSE;
        }
        parse(node, msg.data, msg.len, msg.isrequest);
    }
    pthread_exit(NULL);
}

static void parse(NODE_T *node, void *data, int len, int isrequest)
{
    int flag = 0;
    char *tmp = data, *p = data, *q;
    struct timespec start, end;

    assert(node);
    assert(data);
    assert(len > 0);
    assert(isrequest == TRUE || isrequest == FALSE);
    tmp[len] = '\0';
    /* DEBUG("port: %d\nisrequest: %d\ntmp: %s\n", node->port, isrequest, tmp); */

    /* smtp用\r\n.\r\n来判断结束 */
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    if (node->port == SMTP) {
        assert(isrequest == TRUE);
        if (strncmp(tmp, "DATA", 4) == 0) {
            node->isncomplete = TRUE;
            node->mail = NULL;
            goto free;
        }
        if (node->isncomplete == TRUE) {
            /* 如果包的最后是\r\n.\r\n就置为FALSE */
            if (strncmp(tmp + (len - 3), "\r\n.", 3) == 0 ||
                strncmp(tmp + (len - 5), "\r\n.\r\n", 5) == 0) {
                len -= 5;
                tmp[len + 1] = '\0';
                flag = parse_mime(node, tmp, len);
                if (flag == 0) {
                    monitor_push(monitor_queue[PROTO_NOWEB], node->mail, node->port);
                }
                node->isncomplete = FALSE;
                goto free;
            }
            flag = parse_mime(node, tmp, len);
            if (flag < 0) {
                node->isncomplete = FALSE;
            }
        }
        /* node->isncomplete == FALSE */
        goto free;
    }

    if (node->port == IMAP) {
        assert(isrequest == FALSE);
        if (len > 11) {
            if (*p == '*' && strstr(p, "FETCH")) {
                p = strchr(tmp, '\r');
                *p = '\0';
                if (!(p = strstr(tmp, "BODY[]")))
                    goto free;
                p = strchr(tmp, '}');
                node->isncomplete = TRUE;
                p += 3;
                node->mail = NULL;
            }
        }
        if (node->isncomplete == FALSE)
            goto free;
        /* p在第一行最后，最后有 )\r\nnum OK Fetch completed\r\n, 并在)处中断 */
        if (strncmp(tmp + (len - 11), "completed\r\n", 11) == 0) {
            if (len >= 27) {
                DEBUG("len: %d, newlen: %d, p: %s\n", len, (int)(p - tmp), p);
                len -= (int) (p - tmp);
                q = strchr(p + len - 27, ')');
                if (q) {
                    *q = '\0';
                    DEBUG("tmp: %s\n", tmp);
                    len -= (int) (p + len - q);
                    flag = parse_mime(node, p, len);
                }
            }
            if (flag == 0) {
                monitor_push(monitor_queue[PROTO_NOWEB], node->mail, node->port);
            }
            node->isncomplete = FALSE;
            goto free;
        }
        len -= (int) (p - tmp);
        flag = parse_mime(node, p, len);
        if (flag < 0) {
            node->isncomplete = FALSE;
            flag = 0;
        }
        goto free;
    }

    /* 通过长度判断 */
    /* 在相应中+OK length octets */
    if (node->port == POP3) {
        assert(isrequest == FALSE);
        if (strncmp(tmp, "+OK", 3) == 0) {
            if ((p = strstr(tmp, "octets"))) {
                strchr_parseint(tmp + 4, ' ', &node->length);
                DEBUG("node->length: %d\n", node->length);
                node->isncomplete = TRUE;
                goto free;
            }
            node->mail = NULL;
        }
        if (node->isncomplete == TRUE) {
            /* mail */
            node->length -= len;
            if (node->length <= 0) {
                flag = parse_mime(node, tmp, len);
                if (flag == 0) {
                    monitor_push(monitor_queue[PROTO_NOWEB], node->mail, node->port);
                }
                node->isncomplete = FALSE;
                goto free;
            }
            flag = parse_mime(node, tmp, len);
            if (flag < 0) {
                node->isncomplete = FALSE;
            }
        }
        goto free;
    }
free:
    Mpool_free(pkgpool, data);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    //DEBUG("clock: %ld\n", end.tv_nsec - start.tv_nsec);
}

/* 返回0，代表需要监测
 * 返回-1，代表已经封堵完成
 * 返回-2，代表分析中出现错误*/
static int parse_mime(NODE_T *node, void *data, int len)
{
    static int state = STATE_START;
    char *p = data, *tmp;
    char str[128];

    DEBUG("port: %d\nmail:\n%s\n", node->port, (char*)data);
    if (node->mail == NULL) {
        ASSERTRETLABEL(node->mail = Mpool_alloc(mailpool), error);
        MAIL_INIT(node->mail);
        state = STATE_START;

        /* 假定From, To, Subject一定在第一个包中，并且是按顺序的 */
        ASSERTRETLABEL(p = strstr(p, "From: "), error);
        p = strcpychr(p + 6, str, '\r');
        str_decode(str, node->mail->from);
        tmp = match_match(match_use[TYPE_KILL][KEY_FROM], node->mail->from);
        if (tmp) {
            kill_tcp_with_protocol(node->ts, node->port);
            log_push(log_queue[PROTO_NOWEB][TYPE_KILL], node->mail, tmp,
                     node->port);
            DEBUG("from: %s\nkeyword: %s\n", node->mail->from, tmp);
            goto killed;
        }

        ASSERTRETLABEL(p = strstr(p, "To: "), error);
        p = strcpychr(p + 4, str, '\r');
        str_decode(str, node->mail->to);
        tmp = match_match(match_use[TYPE_KILL][KEY_TO], node->mail->to);
        if (tmp) {
            kill_tcp_with_protocol(node->ts, node->port);
            log_push(log_queue[PROTO_NOWEB][TYPE_KILL], node->mail, tmp,
                     node->port);
            DEBUG("to: %s\nkeyword: %s\n", node->mail->to, tmp);
            goto killed;
        }

        ASSERTRETLABEL(p = strstr(p, "Subject: "), error);
        p = strcpychr(p + 9, str, '\r');
        str_decode(str, node->mail->subject);
        tmp = match_match(match_use[TYPE_KILL][KEY_SUBJECT],
                          node->mail->subject);
        if (tmp) {
            kill_tcp_with_protocol(node->ts, node->port);
            log_push(log_queue[PROTO_NOWEB][TYPE_KILL], node->mail, tmp,
                     node->port);
            DEBUG("subject: %s\nkeyword: %s\n", node->mail->subject, tmp);
            goto killed;
        }
        DEBUG("from: %s\nto: %s\nsubject: %s\n",
              node->mail->from, node->mail->to, node->mail->subject);

        /* 算长度 */
        len -= (int) (p - (char*)data);
        state = parse_content(node, p, len, state);
        if (state < 0) {
            goto killed;
        }
        return 0;
    }
    state = parse_content(node, data, len, state);
    if (state < 0) {
        goto killed;
    }
    return 0;
killed:
    return -1;
error:
    return -2;
}

/**
 * 模拟状态机
 * 返回大于0等于0的数，表明正常
 * 返回小于0的数，说明已经封堵或者分析出现错误
 * 只处理base64编码
 */
static int parse_content(NODE_T *node, void *data, int len, int state)
{
    /* tmplen 计算内容，附件长度用，delen解码之后的长度，dlen还剩几个没有解码 */
    /* zlen临时用的总长度 */
    int tmplen, delen, zlen, start = len;
    char *bufp;
    char str[128];
    char *p = data, *tmp, *q = NULL;
    static int dlen = 0;
    static int size = 0;
    static char buf[1500] = {0};
    char debuf[1500] = {0};
    static int encoding = NONE;
    static int type = NONE;

start:
    switch(state) {
    case STATE_START:
        size = 0;
        encoding = NONE;
        type = NONE;
        dlen = 0;
        memset(buf, 0, 1500);
        memset(debuf, 0, 1500);
        ASSERTRETLABEL(p = strstr(p, "Content-Type: "), finished);
        node->type = contenttype(p);
        TRANS(STATE_NOCONTENT);

    case STATE_NOCONTENT:
        if (node->type == CONTENT_TEXT) {
            ASSERTRETLABEL(p = strstr(p, "Content-Transfer-Encoding: "),
                           error);
            encoding = transfer(p);
            ASSERTRETLABEL(p = strstr(p, "\r\n\r\n"), error);
            p += 4;
            len -= (int)(p - (char*)data);
            TRANS(STATE_CONTENTMID);
        }

        if (node->type == CONTENT_ALTERNATIVE) {
            p += 14;
            ASSERTRETLABEL(p = strstr(p, "Content-Type: "), finished);
            type = contenttype(p);
            ASSERTRETLABEL(type == CONTENT_TEXT, error);
            ASSERTRETLABEL(p = strstr(p, "Content-Transfer-Encoding: "),
                           error);
            encoding = transfer(p);
            ASSERTRETLABEL(p = strstr(p, "\r\n\r\n"), error);
            p += 4;
            len -= (int)(p - (char*)data);
            TRANS(STATE_CONTENTMID);
        }

        if (node->type == CONTENT_MIXED) {
            while (1) {
                p += 14;
                ASSERTRETLABEL(p = strstr(p, "Content-Type: "), finished);
                type = contenttype(p);
                if (type != CONTENT_TEXT)
                    continue;
                ASSERTRETLABEL(p = strstr(p, "Content-Transfer-Encoding: "),
                               error);
                encoding = transfer(p);
                ASSERTRETLABEL(p = strstr(p, "\r\n\r\n"), error);
                p += 4;
                len -= (int)(p - (char*)data);
                TRANS(STATE_CONTENTMID);
            }
        }

    case STATE_CONTENTMID:
        if (node->type == CONTENT_TEXT) {
            if (encoding == NONE) {
                bufp = p;
            } else {
                tmplen = len;
                memcpy(buf+dlen, p, tmplen);
                dlen += tmplen;
                zlen = dlen;
                delen = base64_decode_stream(buf, &dlen, debuf);
                debuf[delen] = '\0';
                memmove(buf, buf + zlen - dlen, dlen);
                bufp = debuf;
                tmplen = delen;
            }
            tmp = match_match(match_use[TYPE_KILL][KEY_CONTENT], bufp);
            if (tmp) {
                kill_tcp_with_protocol(node->ts, node->port);
                ASSERTRETLABEL(REALLOC(node->mail->content, node->mail->content,
                                       size + len + 1), error);
                memcpy(node->mail->content + size, bufp, len);
                node->mail->content[size + len] = '\0';
                DEBUG("content: %s\nkeyword: %s\n", node->mail->content, tmp);
                goto killed;
            }
            ASSERTRETLABEL(REALLOC(node->mail->content, node->mail->content,
                                   size + len + 1), error);
            memcpy(node->mail->content + size, bufp, len);
            size += len;
            node->mail->content[size] = '\0';
            DEBUG("content: %s\n", node->mail->content);
            goto finished;
        }

        if (node->type == CONTENT_ALTERNATIVE ||
            node->type == CONTENT_MIXED) {
            /* 需要找结尾 */
            q = strstr(p, "\r\n--");
            if (q) {
                tmplen = (int)(q - p);
            } else {
                tmplen = len;
            }
            if (encoding == NONE) {
                bufp = p;
            } else {
                memcpy(buf+dlen, p, tmplen);
                dlen += tmplen;
                zlen = dlen;
                delen = base64_decode_stream(buf, &dlen, debuf);
                debuf[delen] = '\0';
                DEBUG("delen: %d, zlen: %d, dlen: %d\n", delen, zlen, dlen);
                if (dlen != 0) {
                    memmove(buf, buf + zlen - dlen, dlen);
                }
                bufp = debuf;
                tmplen = delen;
            }
            tmp = match_match(match_use[TYPE_KILL][KEY_CONTENT], bufp);
            if (tmp) {
                kill_tcp_with_protocol(node->ts, node->port);
                ASSERTRETLABEL(REALLOC(node->mail->content, node->mail->content,
                                       size + tmplen + 1), error);
                memcpy(node->mail->content + size, bufp, tmplen);
                node->mail->content[size + tmplen] = '\0';
                DEBUG("content: %s\nkeyword: %s\n", node->mail->content, tmp);
                goto killed;
            }
            ASSERTRETLABEL(REALLOC(node->mail->content, node->mail->content,
                                   size + tmplen + 1), error);
            memcpy(node->mail->content + size, bufp, tmplen);
            size += tmplen;
            node->mail->content[size] = '\0';
        }

        if (node->type == CONTENT_ALTERNATIVE) {
            if (q) {
                /* 为了防止找到结尾之后，后续的包还继续读取 */
                DEBUG("content: %s\n", node->mail->content);
                monitor_push(monitor_queue[PROTO_NOWEB], node->mail, node->port);
                goto error;
            }
            goto finished;
        }
        if (node->type == CONTENT_MIXED) {
            if (p) {
                DEBUG("content: %s\n", node->mail->content);
                TRANS(STATE_ATTACH);
            }
            goto finished;
        }

    case STATE_ATTACH:
        size = 0;
        dlen = 0;
        tmplen = 0;
        encoding = NONE;
        memset(debuf, 0, 1500);

        /* 找文件名 */
        ASSERTRETLABEL(p = strstr(p, "filename"), finished);
        /* 默认base64编码 */
        if (*(p - 38) != '4' && *(p - 36) != '4') {
            encoding = NONE;
        } else {
            encoding = BASE64;
        }
        /* TODO：在此处处理特殊格式 */
        /* 跳过filename=",或者filename*= */
        p += 10;
        if (*(p - 1) == '"') {
            p = strcpychr(p, str, '"');
            /* 跳到\r处 */
            p++;
        } else {
            /* 跳过两个'' */
            ASSERTRETLABEL(p = strchr(p, '\''), error);
            p += 2;
            p = strcpychr(p, str, '\r');
        }
        str_decode(str, node->mail->attach_name);
        tmp = match_match(match_use[TYPE_KILL][KEY_ATTACH_NAME],
                          node->mail->attach_name);
        if (tmp) {
            kill_tcp_with_protocol(node->ts, node->port);
            DEBUG("filename: %s\nkeyword: %s\n", node->mail->attach_name, tmp);
            goto killed;
        }
        DEBUG("filename: %s\n", node->mail->attach_name);
        if (!istxt(node->mail->attach_name)) {
            monitor_push(monitor_queue[PROTO_NOWEB], node->mail, node->port);
            goto error;
        }
        DEBUG("attach ment!\n");
        /* 有可能transfer-encoding在文件名之后 */
        /* p 指向\r\n,Content-Transfer-Encoding */
        if (strncmp(p + 2, "Content-Transfer-Encoding: ", 27) == 0) {
            if (strncmp(p + 29, "base64", 6) == 0) {
                DEBUG("p +27: %s\n", p + 27);
                encoding = BASE64;
            }
        }

        ASSERTRETLABEL(p = strstr(p, "\r\n\r\n"), error);
        p += 4;
        len = start;
        len -= (int)(p - (char*)data);
        TRANS(STATE_ATTACHMID);

    case STATE_ATTACHMID:
        q = strstr(p, "\r\n--");
        if (q) {
            tmplen = (int) (q - p);
        } else {
            tmplen = len;
        }
        if (encoding == NONE) {
            bufp = p;
        } else {
            /* 只解析base64 */
            memcpy(buf + dlen, p, tmplen);
            dlen += tmplen;
            zlen = dlen;
            delen = base64_decode_stream(buf, &dlen, debuf);
            debuf[delen] = '\0';
            memmove(buf, buf + zlen - dlen, dlen);
            bufp = debuf;
            tmplen = delen;
        }
        tmp = match_match(match_use[TYPE_KILL][KEY_ATTACH_CONTENT], bufp);
        if (tmp) {
            kill_tcp_with_protocol(node->ts, node->port);
            ASSERTRETLABEL(REALLOC(node->mail->attach_content,
                                   node->mail->attach_content,
                                   size + tmplen + 1), error);
            memcpy(node->mail->attach_content + size, bufp, tmplen);
            node->mail->attach_content[size + tmplen] = '\0';
            DEBUG("attach_content: %s\nkeyword: %s\n", node->mail->attach_content, tmp);
            goto killed;
        }
        ASSERTRETLABEL(REALLOC(node->mail->attach_content,
                               node->mail->attach_content,
                               size + tmplen + 1), error);
        memcpy(node->mail->attach_content + size, bufp, tmplen);
        size += tmplen;
        node->mail->attach_content[size] = '\0';
        if (q) {
            DEBUG("size: %d\nattach_content: %s\n", size,
                  node->mail->attach_content);
            monitor_push(monitor_queue[PROTO_NOWEB], node->mail, node->port);
            goto error;
        }
        goto finished;

    default:
        return -1;
    }

finished:
    return state;
killed:
    log_push(log_queue[PROTO_NOWEB][TYPE_KILL], node->mail, tmp, node->port);
    return -1;
error:
    return -1;
}

static int contenttype(char *p)
{
    /* p 指向contenttype开头 */
    p += 14;
    if (strncmp(p, "text/plain", 4) == 0) {
        return CONTENT_TEXT;
    }
    if (strncmp(p, "multipart/alternative", 21) == 0) {
        return CONTENT_ALTERNATIVE;
    }
    if (strncmp(p, "multipart/mixed", 15) == 0) {
        return CONTENT_MIXED;
    }
    return CONTENT_TEXT;
}

static int transfer(char *p)
{
    /* p 指向content-transfer-encoding:  */
    p += 27;
    if (strncmp(p, "base64", 6) == 0) {
        return BASE64;
    } else if (strncmp(p, "quoted-printable", 16) == 0) {
        return QUOTED;
    } else {
        return NONE;
    }
}
