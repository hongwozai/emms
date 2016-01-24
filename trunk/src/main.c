/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：main.c
 ** 创建人：路泽亚
 ** 描  述：主控模块
 ** 注  意：1.控制系统起停，与前台通信
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-23
 ********************************************************************/
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "inc/db.h"
#include "inc/log.h"
#include "inc/kill.h"
#include "inc/spam.h"
#include "inc/analy.h"
#include "inc/match.h"
#include "inc/global.h"
#include "inc/monitor.h"

#include "utils/cfg.h"
#include "utils/sock.h"
#include "utils/tpool.h"
#include "utils/debug.h"
#include "utils/monitor.h"
#include "utils/systemlog.h"

#define VERSION 1
#define CONFIG_FILENAME "emms.ini"
#define LOG_FILENAME    "emms.log"
#define MAXLINE 1024
#define FAIL "0"
#define OK   "1"

#define CHECKERR(exp)                           \
    if (!exp) {                                 \
        return -1;                              \
    }

static int  isdaemon;
static int  isboot;
static int  nthread;
static char listen_ip[16];
static short listen_port;
static char netcard[128];
static char spam_data[128];
static double spam_thresold;
static char oracle_servername[128];
static char oracle_username[128];
static char oracle_password[128];

static void verify_authority(void);
static void parse_args(int argc, char *argv[]);
static void ignore_signal(void);
static void print_help(void);
static void load_config(void);
static int  server_start(void);
static void server_run(void);
static void server_stop(void);
static void accept_request(void* data);
static void signal_handle(int signum);

int main(int argc, char *argv[])
{
    int flag;
    int listenfd;

    parse_args(argc, argv);
    ignore_signal();
    verify_authority();
    load_config();
    flag = systemlog_init(isdaemon? OUT_LOGFILE : OUT_SCREEN, LOG_FILENAME);
    if (flag == -1) {
        FATAL("log file can't create!(%s)\n", strerror(errno));
        return 0;
    }

    if (isdaemon)
        daemon(1, 0);
    threadpool = Tpool_new(nthread);
    Tpool_run(threadpool);

    listenfd = anet_tcp_server(listen_ip, listen_port, 0);
    if (listenfd == -1) {
        FATAL("Server Initialize fail!(%s)", strerror(errno));
        goto free;
    }
    INFO("Server Initialize successful!");
    for (;;) {
        long clientfd = anet_tcp_accept(listenfd, 0);
        if (clientfd == -1) {
            FATAL("accept fail!(%s)\n", strerror(errno));
            close(listenfd);
            goto free;
        }
        Tpool_addwork(threadpool, accept_request, (void*)clientfd);
    }
free:
    Tpool_free(threadpool);
    systemlog_close();
    return 0;
}

static void verify_authority(void)
{
    if (getuid() != 0) {
        fprintf(stderr, "User isn't root!\n");
        exit(1);
    }
}

static void ignore_signal(void)
{
    signal(SIGSEGV, signal_handle);
    signal(SIGABRT, signal_handle);
    signal(SIGPIPE, signal_handle);
    signal(SIGCHLD, signal_handle);
    signal(SIGALRM, signal_handle);
    signal(SIGINT, signal_handle);
}

static void parse_args(int argc, char **argv)
{
    int opt;

    /* 命令行解析 */
    opterr = 0;
    while ((opt = getopt(argc, argv, "dh")) != -1) {
        if (opt == 'd') {
            isdaemon = 1;
        } else if (opt == 'h') {
            print_help();
            exit(0);
        } else {
            /* opt == '?' */
            printf("Syntax error: invaild arg %s\n", argv[optind - 1]);
            printf("\n");
            print_help();
            exit(0);
        }
    }
}

static void signal_handle(int signum)
{
    switch(signum) {
    case SIGINT:
        INFO("User press Ctrl-C, Cancel emms!");
        exit(0);
    case SIGSEGV:
        INFO("emms will be SIGSEGV kill!");
        do_backtrace();
        exit(0);
    case SIGABRT:
        INFO("emms will be SIGABRT kill!");
        do_backtrace();
        exit(0);
    default:
        INFO("emms receive %d", signum);
    }
}

static void print_help(void)
{
    printf("EMMS version: %d.0\n", VERSION);
    printf("Usage: emms [options]\n");
    printf("Options: \n");
    printf("-d\t\t以守护进程的方式运行\n");
    printf("-h\t\t获取帮助\n");
}

static void load_config(void)
{
    cfg_t *cfg;
    char listen_port_str[128];
    char ip[128];
    char servername[128];
    char num[128];
    char thresold[128];

    cfg_open(&cfg, CONFIG_FILENAME);
    cfg_read(cfg, "server", "ip", listen_ip);
    cfg_read(cfg, "server", "listen", listen_port_str);
    cfg_read(cfg, "server", "netcard", netcard);
    cfg_read(cfg, "server", "thread num", num);
    cfg_read(cfg, "oracle", "ip", ip);
    cfg_read(cfg, "oracle", "servername", servername);
    cfg_read(cfg, "oracle", "username", oracle_username);
    cfg_read(cfg, "oracle", "password", oracle_password);
    cfg_read(cfg, "spam", "thresold", thresold);
    cfg_read(cfg, "spam", "data file", spam_data);
    cfg_close(cfg);

    /* 处理，应该在此进行正确性判断 */
    sprintf(oracle_servername, "%s/%s", ip, servername);
    listen_port = atoi(listen_port_str);
    nthread = atoi(num);
    spam_thresold = atof(thresold);
    DEBUG("listen ip: %s:%s\n"
          "oracle user: %s, password: %s\n"
          "thread count: %d\nspam_data: %s\n"
          "thresold: %lf\n",
          listen_ip, listen_port_str,
          oracle_username, oracle_password,
          nthread, spam_data, spam_thresold);
}

/* 意大利面条式代码，但是暂时还没什么好的错误处理方法 */
static int server_start(void)
{
    int flag;

#ifdef HAVE_DB
    flag = db_init(oracle_servername, oracle_username, oracle_password);
    if (flag == -1) {
        return -1;
    }
    INFO("Database initilize!");
#endif
    flag = spam_init(spam_thresold, spam_data);
    if (flag == -1) {
        return -1;
    }
    flag = match_init();
    if (flag == -1) {
        return -1;
    }
    flag = kill_init();
    if (flag == -1) {
        match_close();
        return -1;
    }
    flag = log_init();
    if (flag == -1) {
        match_close();
        kill_close();
        return -1;
    }
    flag = monitor_init();
    if (flag == -1) {
        match_close();
        kill_close();
        log_close();
        return -1;
    }
    flag = analy_init(netcard);
    if (flag == -1) {
        match_close();
        kill_close();
        log_close();
        monitor_close();
    }
    return 0;
}

static void server_stop(void)
{
#ifdef HAVE_DB
    db_close();
#endif
    analy_stop();
    monitor_stop();
    log_stop();
    kill_close();
    match_close();
    spam_close();
}

static void server_run(void)
{
    analy_run();
    monitor_run();
    log_run();
}

static void accept_request(void* data)
{
    char recvbuf[MAXLINE];
    char tmpbuf[43];
    int mem_total, disk_all, disk_use;
    int type, keytype;
    int clientfd = (int)(long)data;

    pr_peeraddr(clientfd);
    for (;;) {
        int flag = anet_tcp_readline(clientfd, recvbuf, sizeof(recvbuf), 0);
        if (flag == -1) {
            ERROR_NO("readline error!");
            return;
        } else if (flag == 0) {
            DEBUG("peer close!\n");
            return;
        }
        DEBUG("%s", recvbuf);
        switch(recvbuf[0]) {
        case 'S':
            if (isboot == 1) {
                anet_tcp_send(clientfd, FAIL, strlen(FAIL), 0);
                break;
            }
            flag = server_start();
            if (flag == -1) {
                anet_tcp_send(clientfd, FAIL, strlen(FAIL), 0);
                INFO("Service start fail!");
                break;
            }
            server_run();
            isboot = 1;
            anet_tcp_send(clientfd, OK, strlen(OK), 0);
            INFO("Service start!");
            break;
        case 'E':
            if (isboot == 0) {
                anet_tcp_send(clientfd, FAIL, strlen(FAIL), 0);
                INFO("Service stop fail!");
                break;
            }
            server_stop();
            isboot = 0;
            anet_tcp_send(clientfd, OK, strlen(OK), 0);
            INFO("Service stop!");
            break;
        case 'A':
            if (isboot == 1) {
                type = recvbuf[1] - '0';
                keytype = recvbuf[2] - '0';
                match_rebuild(type, keytype);
                anet_tcp_send(clientfd, OK, strlen(OK), 0);
                break;
            }
            anet_tcp_send(clientfd, FAIL, strlen(FAIL), 0);
            break;
        case 'D':
            if (isboot == 1) {
                type = recvbuf[1] - '0';
                keytype = recvbuf[2] - '0';
                match_rebuild(type, keytype);
                anet_tcp_send(clientfd, OK, strlen(OK), 0);
                break;
            }
            anet_tcp_send(clientfd, FAIL, strlen(FAIL), 0);
            break;
        case 'C':
            sprintf(tmpbuf, "%lf\n", get_cpu());
            anet_tcp_send(clientfd, tmpbuf, strlen(tmpbuf), 0);
            break;
        case 'H':
            sprintf(tmpbuf, "%lf\n", get_disk(&disk_all, &disk_use));
            anet_tcp_send(clientfd, tmpbuf, strlen(tmpbuf), 0);
            break;
        case 'M':
            sprintf(tmpbuf, "%lf\n", get_mem(&mem_total));
            anet_tcp_send(clientfd, tmpbuf, strlen(tmpbuf), 0);
            break;
        default:
            anet_tcp_send(clientfd, FAIL, strlen(FAIL), 0);
        }
    }
    close(clientfd);
}
