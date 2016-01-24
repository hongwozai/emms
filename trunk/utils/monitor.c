/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：monitor.c
 ** 创建人：路泽亚
 ** 描  述：获得系统状态情况
 ** 注  意：1
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-14
 ********************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <mntent.h>
#include <sys/vfs.h>

#define ERROR_NO(str, args...)                      \
    printf(str"(%s)\n", ##args, strerror(errno))

double
get_cpu()
{
    FILE *fp;
    char line[80];
    double state1, state2;
    int  user, system, idle1, idle2;

    fp = fopen("/proc/stat", "r");
    if (!fp) {
        ERROR_NO("open /proc/stat file error!");
        return -1;
    }
    fgets(line, 80, fp);
    /* 第一次 */
    /* 取第一个user，第三个system，第四个idle */
    sscanf(line, "cpu  %11d %*d %11d %11d", &user, &system, &idle1);
    state1 = user + system + idle1;
    /* 间隔, 100ms */
    usleep(100000);
    /* 第二次 */
    rewind(fp);
    fgets(line, 80, fp);
    /* 描述符尽早关闭 */
    fclose(fp);
    sscanf(line, "cpu  %11d %*d %11d %11d", &user, &system, &idle2);
    state2 = user + system + idle2;
    /* 计算，复用第二次的变量 */
    state2 = state2 - state1;
    idle2  = idle2  - idle1;
    state2 = ( (double)( state2 - idle2) / state2 ) * 100;
    return state2;
}

/* 通过/proc/meminfo来判断，也可以通过sysinfo系统调用来获取
 * 由于/proc/meminfo以kB做单位，所以int可以*/
double
get_mem(int *total)
{
    FILE  *fp;
    double ratio;
    char   line[80];
    int    avail;

    if (!total) {
        return -2;
    }
    /* 内存占用的单位为kB */
    fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        ERROR_NO("open /proc/meminfo error!");
        return -1;
    }
    /* 取第一行总数，第三行可利用 */
    fgets(line, 80, fp);
    sscanf(line, "%*s%11d", total);
    fgets(line, 80, fp);
    fgets(line, 80, fp);
    /* 尽早关闭文件描述符 */
    fclose(fp);
    sscanf(line, "%*s%11d", &avail);
    /* 计算 */
    ratio = (double)(*total - avail)/(*total) * 100;
    return ratio;
}

/* 读取/etc/mtab文件（所有的已挂载文件系统）找到所有挂载的文件系统
 * 而后使用statfs读取挂载的大小
 * mtab可以使用setmntent, getmntent，来直接读取格式
 * 传出参数单位为kB
 */
double
get_disk(int *all, int *use)
{
    FILE *fp;
    double ratio;
    struct mntent *mnt;
    struct statfs stat;

    if (!all && !use) {
        return -2;
    }
    *all = 0;
    *use = 0;
    if (!(fp = setmntent("/etc/mtab", "r"))) {
        ERROR_NO("open /etc/mtab fail!");
        return -1;
    }
    /* getmntent内部分配mnt空间 */
    mnt = getmntent(fp);
    while ((mnt = getmntent(fp))) {
        long usage;
        if (-1 == statfs(mnt->mnt_dir, &stat)) {
            ERROR_NO("can't get %s size", mnt->mnt_dir);
            return -1;
        }
        /* 除以1024 */
        usage  = (stat.f_blocks * stat.f_bsize) >> 10;
        *all += usage;
        usage -= ( stat.f_bfree * stat.f_bsize) >> 10;
        *use += usage;
    }
    fclose(fp);
    /* 求磁盘总量 */
    ratio = (double)*use/(*all);
    return ratio;
}
