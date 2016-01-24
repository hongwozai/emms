/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：sock.h
 ** 创建人：路泽亚
 ** 描  述：对socket函数的封装
 ** 注  意：1.该库的封装并不好，需要重新编写
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-14
 **
 ** 版本： v0.9
 ** 作    者：路泽亚
 ** 完成日期： 2015-07-23
 ********************************************************************/
#ifndef MY_UTILS_SOCK_H
#define MY_UTILS_SOCK_H

#include <sys/types.h>
#include <arpa/inet.h>

/* 共用函数封装 */
ssize_t readn(int sockfd, void *buf, size_t count);
ssize_t writen(int sockfd, void *buf, size_t count);
ssize_t readline(int sockfd, void *buf, size_t maxline);

int active_nonblock(int sockfd);
int deactive_nonblock(int sockfd);
int read_timeout(int sockfd, unsigned int sec);
int write_timeout(int sockfd, unsigned int sec);
int connect_timeout(int sockfd, struct sockaddr_in *addr, unsigned int sec);
int accept_timeout(int sockfd, struct sockaddr_in *addr, unsigned int sec);

int getlocalsockaddr(int sockfd, char *ip, int *port);
int pr_sockaddr(int sockfd);
int pr_peeraddr(int sockfd);

/* 客户端与服务器封装 */
int anet_tcp_send(int sockfd, void *buf, size_t buflen, unsigned int sec);
int anet_tcp_recv(int sockfd, void *buf, size_t buflen, unsigned int sec);
int anet_tcp_readline(int sockfd, void *buf, size_t maxline, unsigned int sec);
int anet_tcp_client();
int anet_tcp_connect(int sockfd, const char *ip, short port,
                     unsigned int sec);
int anet_tcp_server(const char *ip, short port, int blacklog);
int anet_tcp_accept(int sockfd, unsigned int sec);

#endif /* MY_UTILS_SOCK_H */
