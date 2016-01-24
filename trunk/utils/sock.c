/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：sock.c
 ** 创建人：路泽亚
 ** 描  述：对socket函数的封装
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-14
 **
 ** 版本： v0.9
 ** 作    者：路泽亚
 ** 完成日期： 2015-07-23
 ********************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/select.h>

#include "systemlog.h"

#define TRUE  0
#define FALSE -1

/* 为了兼容之前代码 */
#define err(str) ERROR(str);
#define debug(str, args...) DEBUG(str"\n", ##args)

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

static void pr_addr(struct sockaddr_in *addr);
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

int
anet_tcp_send(int sockfd, void *buf, size_t buflen, unsigned int sec)
{
    int ret;

    ret = write_timeout(sockfd, sec);
    if (ret == TRUE) {
        return writen(sockfd, buf, buflen);
    }
    /* ret为FALSE时， write_timeout会设置errno */
    return -1;
}

int
anet_tcp_recv(int sockfd, void *buf, size_t buflen, unsigned int sec)
{
    int ret;

    ret = read_timeout(sockfd, sec);
    if (ret == TRUE) {
        return readn(sockfd, buf, buflen);
    }
    return -1;
}

int
anet_tcp_readline(int sockfd, void *buf, size_t maxline, unsigned int sec)
{
    int ret;

    ret = read_timeout(sockfd, sec);
    if (ret == TRUE) {
        return readline(sockfd, buf, maxline);
    }
    return -1;
}

int
anet_tcp_client()
{
    int sockfd;
    int optval;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        err("sockfd failure");
        return -1;
    }
    optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                   &optval, sizeof(optval)) == -1){
        err("address reuse failure");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int
anet_tcp_connect(int sockfd, const char *ip, short port, unsigned int sec)
{
    int ret;
    struct sockaddr_in addr;

    addr.sin_family        =   AF_INET;
    addr.sin_addr.s_addr   =   inet_addr(ip);
    addr.sin_port          =   htons(port);
    ret = connect_timeout(sockfd, &addr, sec);
    return ret;
}

int
anet_tcp_server(const char *ip, short port, int blacklog)
{
    int sockfd, optval;
    struct sockaddr_in sockaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        return -1;
    }
    optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                   &optval, sizeof(optval)) == -1){
        close(sockfd);
        return -1;
    }
    sockaddr.sin_family       =  AF_INET;
    sockaddr.sin_addr.s_addr  =  inet_addr(ip==NULL?"127.0.0.1":ip);
    sockaddr.sin_port         =  htons(port);
    if (port != 0) {
        if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) == -1) {
            close(sockfd);
            return -1;
        }
    }
    if (listen(sockfd, blacklog?blacklog:SOMAXCONN) == -1) {
        close(sockfd);
        return -1;
    }
    return sockfd;
}

int
anet_tcp_accept(int sockfd, unsigned int sec)
{
    return accept_timeout(sockfd, NULL, sec);
}


ssize_t
readn(int sockfd, void *buf, size_t count)
{
    size_t nleft = count;
    char *tmp = (char *)buf;

    while (nleft > 0) {
        ssize_t nread = read(sockfd, tmp, nleft);
        if (nread == -1) {
            if (errno == EINTR)
                continue;
            return -1;
        } else if (nread == 0) {
            /* 对方关闭 */
            return count - nleft;
        } else {
            nleft -= nread;
            tmp   += nread;
        }
    }
    return count;
}

ssize_t
writen(int sockfd, void *buf, size_t count)
{
    size_t nleft = count;
    char *tmp = (char *)buf;

    while (nleft > 0) {
        ssize_t nwrite = write(sockfd, tmp, nleft);
        if (nwrite == -1) {
            if (errno == EINTR)
                continue;
            return -1;
        } else if (nwrite == 0) {
            /* 对方关闭 */
            return count - nleft;
        } else {
            nleft -= nwrite;
            tmp   += nwrite;
        }
    }
    return count;
}

ssize_t
readline(int sockfd, void *buf, size_t maxline)
{
    int i;
    int ret = -1;
    char *bufp = (char *)buf;
    int nleft = maxline;

    while (nleft > 0) {
        int nread;
        ret = recv(sockfd, bufp, nleft, MSG_PEEK);
        if (ret < 0) {
            return -1;
        } else if (ret == 0) {
            return 0;
        }
        /* 看看有无\n，然后再做决定 */
        nread = ret;
        for (i = 0; i < ret; i++) {
            if (bufp[i] == '\n') {
                /* 有\n, 直接返回 */
                ret = readn(sockfd, bufp, i+1);
                /* 因为已经在缓冲区内了，所以ret不等于i+1就一定错误 */
                if (ret != i+1) {
                    return -1;
                }
                /* 读到一行, 返回 */
                return ret;
            }
        }
        /* 无\n, 将buf中的全部读完后，继续再读(循环) */
        ret = readn(sockfd, bufp, nread);
        if (ret != nread) {
            return -1;
        }
        nleft -= nread;
        bufp += nread;
    }
    return ret;
}

int
active_nonblock(int sockfd)
{
    int ret;
    int flag = fcntl(sockfd, F_GETFL);
    if (flag == -1)
        return FALSE;
    flag |= O_NONBLOCK;
    ret = fcntl(sockfd, F_SETFL, flag);
    if (ret == -1)
        return FALSE;
    return TRUE;
}

int
deactive_nonblock(int sockfd)
{
    int ret;
    int flag = fcntl(sockfd, F_GETFL);
    if (flag == -1)
        return FALSE;
    flag &= ~O_NONBLOCK;
    ret = fcntl(sockfd, F_SETFL, flag);
    if (ret == -1)
        return FALSE;
    return TRUE;
}

int
read_timeout(int sockfd, unsigned int sec)
{
    int ret;
    fd_set rset;
    void *timeout;
    struct timeval tval;

    FD_ZERO(&rset);
    FD_SET(sockfd, &rset);
    tval.tv_sec  = sec;
    tval.tv_usec = 0;
    timeout = sec?&tval:NULL;
    do {
        ret = select(sockfd+1, &rset, NULL, NULL, timeout);
    } while (ret == -1 && errno == EINTR);
    /* timeout */
    if (ret == 0) {
        errno = ETIMEDOUT;
        return FALSE;
    }
    if (ret == -1)
        return FALSE;
    return TRUE;
}

int
write_timeout(int sockfd, unsigned int sec)
{
    int ret;
    fd_set wset;
    void *timeout;
    struct timeval tval;

    FD_ZERO(&wset);
    FD_SET(sockfd, &wset);
    tval.tv_sec  = sec;
    tval.tv_usec = 0;
    timeout = sec?&tval:NULL;
    do {
        ret = select(sockfd+1, NULL, &wset, NULL, timeout);
    } while (ret == -1 && errno == EINTR);
    /* timeout */
    if (ret == 0) {
        errno = ETIMEDOUT;
        return FALSE;
    }
    /* other error */
    if (ret == -1)
        return FALSE;
    return TRUE;
}

int
connect_timeout(int sockfd, struct sockaddr_in *addr, unsigned int sec)
{
    int ret;

    if (active_nonblock(sockfd) == FALSE)
        goto error;
    ret = connect(sockfd, (struct sockaddr*)addr,
                  sizeof(struct sockaddr_in));
    if (ret == -1 && errno == EINPROGRESS) {
        int sret;
        fd_set wset;
        struct timeval tval;
        void *timeout;

        FD_ZERO(&wset);
        FD_SET(sockfd, &wset);
        tval.tv_sec  = sec;
        tval.tv_usec = 0;
        timeout = sec?&tval:NULL;
        do {
            sret = select(sockfd+1, NULL, &wset, NULL, timeout);
        } while (sret == -1 && errno == EINTR);
        if (sret == -1)
            goto error;
        else if (sret == 0) {
            errno = ETIMEDOUT;
            goto error;
        } else if (sret == 1) {
            int optval;
            socklen_t optlen = sizeof(optval);

            if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR,
                           &optval, &optlen) != 0) {
                goto error;
            }
            if (optval != 0) {
                errno = optval;
                goto error;
            }
        }
    } else if (ret == -1) {
        goto error;
    }
    if (deactive_nonblock(sockfd) == FALSE)
        goto error;
    return TRUE;
error:
    return FALSE;
}

int
accept_timeout(int sockfd, struct sockaddr_in *addr, unsigned int sec)
{
    int ret = 0;
    fd_set rset;
    struct timeval time;
    void *timeout;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    FD_ZERO(&rset);
    FD_SET(sockfd, &rset);
    time.tv_sec = sec;
    time.tv_usec = 0;
    timeout = sec?&time:NULL;
    do {
        ret = select(sockfd+1, &rset, NULL, NULL, timeout);
    } while (ret == -1 && errno == EINTR);
    if (ret == -1) {
        goto error;
    } else if (ret == 0) {
        ret = -1;
        errno = ETIMEDOUT;
    } else {
        ret = accept(sockfd, (struct sockaddr*)addr,
                     addr?&addrlen:NULL);
        /* 可以直接返回ret */
    }
    return ret;
error:
    return -1;
}

static void
pr_addr(struct sockaddr_in *addr)
{
    debug("ip: %s:%d", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
}

int
getlocalsockaddr(int sockfd, char *ip, int *port)
{
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    if (getsockname(sockfd, (struct sockaddr*)&addr, &addrlen) == -1) {
        err("getsockname failure");
        return FALSE;
    }
    strcpy(ip, inet_ntoa(addr.sin_addr));
    *port = ntohs(addr.sin_port);
    return TRUE;
}

int
pr_sockaddr(int sockfd)
{
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    if (getsockname(sockfd, (struct sockaddr*)&addr, &addrlen) == -1) {
        err("getsockname failure");
        return FALSE;
    }
    pr_addr(&addr);
    return TRUE;
}

int
pr_peeraddr(int sockfd)
{
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    if (getpeername(sockfd, (struct sockaddr*)&addr, &addrlen) == -1) {
        err("getpeername failure");
        return FALSE;
    }
    pr_addr(&addr);
    return TRUE;
}
