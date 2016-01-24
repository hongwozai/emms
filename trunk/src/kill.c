/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：kill.c
 ** 创建人：路泽亚
 ** 描  述：封堵模块的实现
 ** 注  意：1.使用原始套接字进行发包
 **        2.同时向客户端与服务器发送数据包
 **        3.为保证封堵效果，发送两遍
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-11-27
 ********************************************************************/
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "inc/kill.h"
#include "inc/global.h"
#include "utils/tpool.h"
#include "utils/tcpip.h"
#include "utils/systemlog.h"

/* 原始套接字 */
static        int         kill_sockfd;
static struct sockaddr_in kill_addr;

static inline void build_tcp_rst(void *buf, uint16_t sp, uint16_t dp,
                                 uint32_t seq, uint32_t saddr, uint32_t daddr)
    __attribute__((always_inline));

static void kill_tcp_request_1(struct tcp_stream *ts);
static void kill_tcp_response_1(struct tcp_stream *ts);

int kill_init()
{
    const int on = 1;

    kill_sockfd = socket(AF_INET, SOCK_RAW, 6);
    if (kill_sockfd == -1)
        goto error;

    if (setsockopt(kill_sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)))
        goto error;

    kill_addr.sin_family = AF_INET;
    return 0;
 error:
    FATAL("module kill can't initialize!(%s)", strerror(errno));
    return -1;
}

void kill_tcp_with_protocol(struct tcp_stream *ts, int proto)
{
    if (proto == SMTP) {
#ifdef KILL_WITH_TPOOL
        Tpool_addwork(threadpool, (void (*)(void*))kill_tcp_request_1, ts);
#else
        kill_tcp_request_1(ts);
#endif
        return;
    }
#ifdef KILL_WITH_TPOOL
        Tpool_addwork(threadpool, (void (*)(void*))kill_tcp_response_1, ts);
#else
        kill_tcp_response_1(ts);
#endif
}

void kill_tcp_request(struct tcp_stream *ts)
{
#ifdef KILL_WITH_TPOOL
    Tpool_addwork(threadpool, (void (*)(void*))kill_tcp_request_1, ts);
#else
    kill_tcp_request_1(ts);
#endif
}

void kill_tcp_response(struct tcp_stream *ts)
{
#ifdef KILL_WITH_TPOOL
    Tpool_addwork(threadpool, (void (*)(void*))kill_tcp_response_1, ts);
#else
    kill_tcp_response_1(ts);
#endif
}

void kill_tcp_request_1(struct tcp_stream *ts)
{
    char buf[40];
    const int client_seq = ts->client.first_data_seq + ts->server.count;
    const int server_seq = ts->server.first_data_seq + ts->client.count;
    const int client_window = ts->server.window;
    const int server_window = ts->client.window;
    register uint32_t saddr = ts->addr.saddr;
    register uint32_t daddr = ts->addr.daddr;
    register uint16_t source = ts->addr.source;
    register uint16_t dest   = ts->addr.dest;

    /* 发往服务器端 */
    kill_addr.sin_addr.s_addr = daddr;
    kill_addr.sin_port        = htons(dest);
    build_tcp_rst(buf + 20, source, dest, client_seq, saddr, daddr);
    build_ipv4_default(buf, 40, 6, saddr, daddr);
    sendto(kill_sockfd, buf, 40, 0,
           (struct sockaddr*)&kill_addr, sizeof(kill_addr));

    /* 发往服务器端 */
    build_tcp_rst(buf + 20, source, dest, client_seq + (client_window >> 1),
                  saddr, daddr);
    sendto(kill_sockfd, buf, 40, 0,
           (struct sockaddr*)&kill_addr, sizeof(kill_addr));

    /* 发往客户端 */
    kill_addr.sin_addr.s_addr = saddr;
    kill_addr.sin_port        = htons(source);
    build_tcp_rst(buf + 20, dest, source, server_seq, daddr, saddr);
    build_ipv4_default(buf, 40, 6, daddr, saddr);
    sendto(kill_sockfd, buf, 40, 0,
           (struct sockaddr*)&kill_addr, sizeof(kill_addr));

    /* 发往客户端 */
    build_tcp_rst(buf + 20, dest, source, server_seq + (server_window >> 1),
                  daddr, saddr);
    sendto(kill_sockfd, buf, 40, 0,
           (struct sockaddr*)&kill_addr, sizeof(kill_addr));
}


void kill_tcp_response_1(struct tcp_stream *ts)
{
    char buf[40];
    const int client_seq = ts->client.first_data_seq + ts->server.count;
    const int server_seq = ts->server.first_data_seq + ts->client.count;
    const int client_window = ts->server.window;
    const int server_window = ts->client.window;
    register uint32_t saddr = ts->addr.saddr;
    register uint32_t daddr = ts->addr.daddr;
    register uint16_t source = ts->addr.source;
    register uint16_t dest   = ts->addr.dest;

    /* 发往客户端 */
    kill_addr.sin_addr.s_addr = saddr;
    kill_addr.sin_port        = htons(source);
    build_tcp_rst(buf + 20, dest, source, server_seq, daddr, saddr);
    build_ipv4_default(buf, 40, 6, daddr, saddr);
    sendto(kill_sockfd, buf, 40, 0,
           (struct sockaddr*)&kill_addr, sizeof(kill_addr));

    /* 发往客户端 */
    build_tcp_rst(buf + 20, dest, source, server_seq + (server_window >> 1),
                  daddr, saddr);
    sendto(kill_sockfd, buf, 40, 0,
           (struct sockaddr*)&kill_addr, sizeof(kill_addr));

    /* 发往服务器端 */
    kill_addr.sin_addr.s_addr = daddr;
    kill_addr.sin_port        = htons(dest);
    build_tcp_rst(buf + 20, source, dest, client_seq, saddr, daddr);
    build_ipv4_default(buf, 40, 6, saddr, daddr);
    sendto(kill_sockfd, buf, 40, 0,
           (struct sockaddr*)&kill_addr, sizeof(kill_addr));

    /* 发往服务器端 */
    build_tcp_rst(buf + 20, source, dest, client_seq + (client_window >> 1),
                  saddr, daddr);
    sendto(kill_sockfd, buf, 40, 0,
           (struct sockaddr*)&kill_addr, sizeof(kill_addr));


}

void kill_close()
{
    close(kill_sockfd);
}


static inline void build_tcp_rst(void *buf, uint16_t sp, uint16_t dp,
                                 uint32_t seq, uint32_t saddr, uint32_t daddr)
{
    struct tcphdr *tcp_header = (struct tcphdr*)buf;

    tcp_header->source  = htons(sp);
    tcp_header->dest    = htons(dp);
    tcp_header->seq     = htonl(seq);
    tcp_header->ack_seq = 0;
    tcp_header->res1    = 0;
    tcp_header->doff    = 5;    /* 首部长度 */
    tcp_header->fin     = 0;
    tcp_header->syn     = 0;
    tcp_header->psh     = 0;
    tcp_header->ack     = 0;
    tcp_header->urg     = 0;
    tcp_header->rst     = 1;
    tcp_header->res2    = 0;
    tcp_header->window  = 0;
    tcp_header->check   = 0;
    tcp_header->urg_ptr = 0;
    /* 校验 */
    tcp_header->check = tcp_chksum(buf, 20, saddr, daddr);
}
