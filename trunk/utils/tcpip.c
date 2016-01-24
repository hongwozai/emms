/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：tcpip.c
 ** 创建人：路泽亚
 ** 描  述：伪造tcp，ip包的头文件
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-10-12
 ********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "tcpip.h"

uint16_t
ipv4_chksum(void *addr, uint16_t count)
{
    register unsigned int sum = 0;
    uint16_t *addr16 = (uint16_t*)addr;

    /**
     * 每16位相加（相加的进位需要加回）
     * 最后求反
     */
    while (count > 1) {
        sum += *addr16++;
        /* 这样做防止需要加的数字太多,使sum超过32位 */
        if (sum >= 0x10000) {
            sum = (sum & 0xffff) + (sum >> 16);
        }
        count -= 2;
    }
    if (count > 0) {
        sum += *(uint8_t *)addr16;
    }
    return ~(uint16_t)sum;
}

uint16_t
tcp_chksum(void *addr, uint16_t count,
           in_addr_t src_ip, in_addr_t dest_ip)
{
    register unsigned int sum = 0;
    uint16_t *addr16 = (uint16_t*)addr;
    int tcp_len = count;

    while (count > 1) {
        sum += *addr16++;
        if (sum >= 0x10000) {
            sum = (sum & 0xffff) + (sum >> 16);
        }
        count -= 2;
    }
    if (count > 0) {
        sum += *(uint8_t *)addr16;
    }
    sum += (src_ip & 0xffff) + (src_ip >> 16);
    sum += (dest_ip & 0xffff) + (dest_ip >> 16);
    sum += htons(0x06);
    sum += htons(tcp_len);
    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }
    return ~sum;
}

void build_ipv4_default(void *buf, uint16_t tot_len, uint8_t proto,
                        uint32_t saddr, unsigned int daddr)
{
    struct iphdr *ip_header = (struct iphdr*)buf;
    ip_header->version  = 4;
    ip_header->ihl      = 0x5;
    ip_header->tos      = 0;
    ip_header->tot_len  = htons(tot_len);
    ip_header->id       = 0;
    ip_header->frag_off = htons(0x4000); /* df位置1 */
    ip_header->ttl      = 64;
    ip_header->protocol = proto;
    ip_header->check    = 0;
    ip_header->saddr    = saddr;
    ip_header->daddr    = daddr;
}

void build_tcp_default(void *buf, uint16_t sp, uint16_t dp,
                       uint32_t seq, uint32_t ack_seq, uint8_t control,
                       uint16_t window, uint32_t saddr, uint32_t daddr)
{
    uint16_t sum;
    struct tcphdr *tcp_header = (struct tcphdr*)buf;

    tcp_header->source  = htons(sp);
    tcp_header->dest    = htons(dp);
    tcp_header->seq     = htonl(seq);
    tcp_header->ack_seq = htonl(ack_seq);
    tcp_header->res1    = 0;
    tcp_header->doff    = 5;    /* 首部长度 */
    tcp_header->fin     = 0;
    tcp_header->syn     = 0;
    tcp_header->rst     = 0;
    tcp_header->psh     = 0;
    tcp_header->ack     = 0;
    tcp_header->urg     = 0;
    /* control 数据的分析 */
    (control & 0x01) && (tcp_header->fin = 1);
    (control & 0x02) && (tcp_header->syn = 1);
    (control & 0x04) && (tcp_header->rst = 1);
    (control & 0x08) && (tcp_header->psh = 1);
    (control & 0x10) && (tcp_header->ack = 1);
    (control & 0x20) && (tcp_header->urg = 1);
    tcp_header->res2    = 0;
    tcp_header->window  = htons(window);
    tcp_header->check   = 0;
    tcp_header->urg_ptr = 0;
    /* 校验 */
    sum = tcp_chksum(buf, 20, saddr, daddr);
    tcp_header->check   = sum;
}
