/********************************************************************
 ** Copyright(c) 2016,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：fileutils.c
 ** 创建人：路泽亚
 ** 描  述：文件处理相关函数
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2016-01-17
 ********************************************************************/
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "fileutils.h"

#define MAX_FILENAME 10240

/* 1为是， 0为否 */
int isdir(const char *dirname)
{
    struct stat st;

    assert(dirname);
    stat(dirname, &st);
    if (S_ISDIR(st.st_mode)) {
        return 1;
    }
    return 0;
}

int walk_files(const char *dirname,
               void (*walk)(const char *filename))
{
    DIR *dir;
    char tmp[MAX_FILENAME];
    struct dirent *dirent;

    /* 打开目录，列出所有文件（不包括目录） */
    assert(walk);
    assert(dirname);

    dir = opendir(dirname);
    if (!dir) {
        fprintf(stderr, "%s: can't not open!(%s)\n",
                dirname, strerror(errno));
        return -1;
    }
    while ((dirent = readdir(dir))) {
        sprintf(tmp, "%s/%s", dirname, dirent->d_name);
        if (!isdir(tmp)) {
            walk(tmp);
        }
    }
    closedir(dir);
    return 0;
}
