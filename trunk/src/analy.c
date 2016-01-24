/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：analy.c
 ** 创建人：路泽亚
 ** 描  述：解析还原模块的主程序
 ** 注  意：1. 解析还原模块的主程序中主要是抓包的部分，而后调用对应的协议解析部分
 **        2. 本文件主要是抓包线程的运行
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
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "inc/analy.h"
#include "inc/global.h"
#include "utils/systemlog.h"
#include "inc/analy_noweb.h"
#include "inc/analy_webmail.h"

#define ADDR2STRING(addr) ( inet_ntoa(*((struct in_addr*)(&(addr)))) )

static pthread_t capture;
static struct nids_chksum_ctl chksum;

static void  nids_callback(struct tcp_stream *ts, void **param);
static void* capture_callback(void *data);

int analy_init(char *device)
{
    nids_params.device = device;
    nids_params.pcap_filter =
        "tcp port 80 or tcp port 143 or tcp port 25 or tcp port 110";
    /* 多线程支持 */
    nids_params.multiproc = 1;

    /* 关闭网卡校验 */
    chksum.netaddr = 0;
    chksum.mask    = 0;
    chksum.action  = NIDS_DONT_CHKSUM;
    nids_register_chksum_ctl(&chksum, 1);

    /* 为mailpool分配 */
    mailpool = Mpool_new(sizeof(mail_t), POOL_BLOCK);
    if (!mailpool) {
        return -1;
    }
    if (!nids_init()) {
        Mpool_dispose(mailpool);
        FATAL("libnids can't initialize!(%s)", nids_errbuf);
        return -1;
    }
    nids_register_tcp(nids_callback);

    /* 各个协议分析的部分 */
    analy_webmail_init();
    analy_noweb_init();
    return 0;
}

int analy_run(void)
{
    int flag;

    if ((flag = pthread_create(&capture, NULL, capture_callback, NULL))) {
        ERROR("thread capture can't create!(%s)", strerror(flag));
        return -1;
    }

    /* 各个协议运行部分 */
    analy_webmail_run();
    analy_noweb_run();
    return 0;
}

void analy_stop(void)
{
    int flag;

    assert(capture);
    if ((flag = pthread_cancel(capture))) {
        ERROR("thread capture can't cancel!(%s)", strerror(flag));
        return;
    }
    /* 如果不等待，一部分内存就会泄漏，并且被取消的线程即使异步也要等待一会才能停止
     * 所以需要等待 */
    pthread_join(capture, NULL);

    /* 各个协议停止部分 */
    analy_webmail_stop();
    analy_noweb_stop();
    analy_close();
}

void analy_close(void)
{
    Mpool_dispose(mailpool);
}

static void* capture_callback(void *data)
{
    (void)data;

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    nids_run();
    DEBUG("capture thread stop!\n");
    pthread_exit(NULL);
}

static void nids_callback(struct tcp_stream *ts, void **param)
{
    (void)param;
    unsigned char *p = (unsigned char*)&ts->addr.daddr;
    int port  = ts->addr.dest;
    int state = ts->nids_state;

    if (state == NIDS_JUST_EST) {
        ts->client.collect++;
        ts->server.collect++;
    }
    if (state == NIDS_DATA || state == NIDS_RESET ||
        state == NIDS_CLOSE) {
        if (port == WEBMAIL) {
            if ((p[0] == 123 && p[1] == 125 && p[2] == 50) ||
                (p[0] == 121 && p[1] == 195 && p[2] == 178)||
                (p[0] == 176 && p[1] == 34 && p[2] == 63)) {
                analy_webmail_putin(ts);
            }
        } else if (port == IMAP || port == SMTP || port == POP3) {
            analy_noweb_putin(ts);
        }
    }
}
