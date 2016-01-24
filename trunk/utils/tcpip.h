/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：tcpip.h
 ** 创建人：路泽亚
 ** 描  述：伪造tcp,ip包的头文件
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-10-12
 ********************************************************************/
#ifndef TCPIP_H
#define TCPIP_H

#include <sys/types.h>
#include <netinet/in.h>

/**
 * ip,tcp头定义，使用netinet/ip.h,netinet/tcp.h
 * 都没有定义ip选项字段, 使用时需要重新定义一个
 * 因为是字节的倍数，所以无需packed
 * 因为定义位域的内存结构涉及大端小端,所以尽量使用系统自带结构体
 * ip使用iphdr, tcp使用tcphdr
 */
#include <netinet/ip.h>
#include <netinet/tcp.h>


/**
 * 定义tcp种控制字段的几个宏
 */
#define TCPFLAG_URG 0x20
#define TCPFLAG_ACK 0x10
#define TCPFLAG_PSH 0x08
#define TCPFLAG_RST 0x04
#define TCPFLAG_SYN 0x02
#define TCPFLAG_FIN 0x01


/**
 * 校验码,ip,icmp可直接使用(长度为20)，tcp/udp/需要加上12字节伪首部
 * 采取和的反码方式
 * @param addr 需要校验的地址,注意校验和部分全填0
 * @param count 需要校验的字节数，ip是20字节
 * @return 返回校验码的值,不必再判断大端小端
 */
extern uint16_t ipv4_chksum(void *addr, uint16_t count);

/**
 * tcp校验和计算函数, 构造了12字节的伪首部
 * 为了效率，直接复制net_chksum的实现, 再加上伪首部
 * @param addr tcp包头第一个字节的地址
 * @param count tcp总长度
 * @param src_ip 必须是大端序的源地址
 * @param dest_ip 必须是大端序的目的地址
 * @return tcp的校验和, 不用再转换字节序
 */
extern uint16_t tcp_chksum(void *addr, uint16_t count, in_addr_t src_ip,
                           in_addr_t dest_ip);

/**
 * 使用默认的方式构造ipv4包
 * 默认：不带选项字段，头部20字节，服务类型均为0, 无分片
 *      由内核分配标识字段与校验和(标识符可能为0)
 * @param buf 需要写入的地址, 需要确保buf有20个字节(只可多不可少)
 * @param tot_len ip包的总长度（应该先构造ip所携带的协议包）
 * @param proto 此ip携带什么协议
 * @param saddr 原地址，应该是大端序的ip地址
 * @param daddr 目的地址，应该是大端序的ip地址
 */
extern void build_ipv4_default(void *buf, uint16_t tot_len, uint8_t proto,
                              uint32_t saddr, unsigned int daddr);

/**
 * 构造默认的tcp包
 * 默认：不带紧急数据, 头部20字节
 * @param buf 要写入的tcp首部
 * @param sp  源端口， 正常序列，函数内部转换字节序
 * @param dp  目的端口, 正常序列，函数内部转换字节序
 * @param seq 序号,正常序列， 函数内部转换字节序
 * @param ack_seq 确认号, 正常序列,函数内部转换字节序
 * @param control 有TCP_URG, ACK,PSH,RST,SYN,FIN等六种
 * @param window  窗口值，正常序列，函数内部转换字节序
 * @param saddr   构造伪首部时的原ip地址, 需要转换好的字节序
 * @param daddr   构造伪首部时的目的ip地址，需要转换好的字节序
 */
extern void build_tcp_default(void *buf, uint16_t sp, uint16_t dp,
                             uint32_t seq, uint32_t ack_seq, uint8_t control,
                             uint16_t window, uint32_t saddr, uint32_t daddr);

#endif /* TCPIP_H */
