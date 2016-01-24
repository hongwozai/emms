/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：base64.c
 ** 创建人：路泽亚
 ** 描  述：base64编码解码的实现
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-10-03
 ********************************************************************/
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "base64.h"

/**
 * base64编码的码表, base64 -> acsii
 * 之所以直接使用全局变量，是为了查表时足够快，而不是节省空间
 */
static char base[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '+', '/'
};

/**
 * base64解码的码表, ascii->base64
 * 需要注意的是从0开始（编码时不用注意）
 */
static unsigned char esab[] = {
    0,                          /* acsii码表是从第0个开始的 */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62,  0,  0,  0, 63, 52,
    53, 54, 55, 56, 57, 58, 59, 60, 61, 0,  0,  0,  0,  0,  0,  0,
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0,  0,  0,  0,  0,  0,
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35,36, 37, 38, 39, 40, 41,
    42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

inline static int base64_equal_sign(char *ch4);

/**
 * 编码步骤：
 * 1. 每三个字节为1组, 共24位
 * 2. 三个字节分为4组，每组6位
 * 3. 每组开头加上两个0，变为32位, 四个字节
 * 4. 按照编码表，进行编码
 * 注：剩两个字节，分三组，最后一字节前后都加两个0, 编码成功后的字符加一个=
 *    剩一个字节，分两组，最后一个字节后加4个0，编码成功后的字符加两个=
 */
int
base64_encode(void *str, int slen, char *code, int clen)
{
    int ccnt = 0;                           /* code已经转换的长度 */
    unsigned char *p = (unsigned char*)str; /* 指向还未处理的数据 */
    char *q = code;             /* 指向传入的内存中还未填充的空间 */
    register unsigned char temp1b = 0;
    register unsigned char temp2b = 0;
    register unsigned char temp3b = 0;

    if (slen == 0 || str == NULL || code == NULL ||
        clen < slen*3/4.0) {
        return 0;
    }
    /* 将三个字节倍数的处理完成 */
    while (slen > 2) {
        temp1b = *(p++);
        temp2b = *(p++);
        temp3b = *(p++);
        q[0] = temp1b >> 2;
        q[1] = ((temp1b << 4) & 0x3f) | (temp2b >> 4);
        q[2] = ((temp2b << 2) & 0x3f) | (temp3b >> 6);
        q[3] = temp3b & 0x3f;
        /* 理论上强制转换为unsigned char最好
         * 不过由于q[0,1,2,3]均不会超过128，因为都是ascii
         * 所以可以强制转换为int。
         * 强制转换为int的问题是可能有负值*/
        q[0] = base[(int)q[0]];
        q[1] = base[(int)q[1]];
        q[2] = base[(int)q[2]];
        q[3] = base[(int)q[3]];
        q += 4;
        slen -= 3;
        ccnt += 4;
    }
    /* 处理剩下的一或二字节 */
    if (slen == 1) {
        temp1b = *(p++);
        q[0] = temp1b >> 2;
        q[1] = (temp1b << 4) & 0x3f;
        /* 查表 */
        q[0] = base[(int)q[0]];
        q[1] = base[(int)q[1]];
        q[2] = '=';
        q[3] = '=';
        q += 4;
        ccnt += 4;
    }
    if (slen == 2) {
        temp1b = *(p++);
        temp2b = *(p++);
        q[0] = temp1b >> 2;
        q[1] = ((temp1b << 4) & 0x3f) | ((temp2b & 0xf0) >> 4);
        q[2] = (temp2b << 2) & 0x3f;
        /* 查表 */
        q[0] = base[(int)q[0]];
        q[1] = base[(int)q[1]];
        q[2] = base[(int)q[2]];
        q[3] = '=';
        q += 4;
        ccnt += 4;
    }
    *q = '\0';
    return ccnt;
}

/**
 * 解码步骤
 * 先判断有几个=号
 * 一、没有等号
 *    1. 第一个字节后6位，第二个字节34位，组成一个字节
 *    2. 第二个字节后4位，第三个字节3456位，组成一个字节
 *    3. 第三个字节后2位，第四个字节后6位，组成一个字节
 * 二、有一个等号
 *    1. 第一个字节后6位，第二个字节34位，组成一个字节
 *    2. 第二个字节后4位，第三个字节3456位，组成一个字节
 * 三、有两个等号
 *    1.第一个字节后6位，第二个字节34位，组成一个字节
 */
int
base64_decode(char *code, int clen, void *str, int slen)
{
    int scnt = 0;                           /* str解码后的长度 */
    char *p = code;                         /* 始终指向未解码的部分 */
    unsigned char *q = (unsigned char*)str; /* 始终指向传入地址空间的 */
    register unsigned char temp1b = 0;
    register unsigned char temp2b = 0;
    register unsigned char temp3b = 0;
    register unsigned char temp4b = 0;

    if (clen == 0 || str == NULL || code == NULL ||
        slen == 0) {
        return 0;
    }
    /* 用a&(b-1)代替mod*/
    if (clen & 3) {
        return 0;
    }
    scnt = 0;
    while (clen > 0 && base64_equal_sign(p) == 0) {
        /* 没有等号 */
        temp1b = *(p++);
        temp2b = *(p++);
        temp3b = *(p++);
        temp4b = *(p++);
        /* 查表 */
        temp1b = esab[(int)temp1b];
        temp2b = esab[(int)temp2b];
        temp3b = esab[(int)temp3b];
        temp4b = esab[(int)temp4b];
        q[0] = (temp1b << 2) | (temp2b >> 4);
        q[1] = (temp2b << 4) | (temp3b >> 2);
        q[2] = (temp3b << 6) | temp4b;
        q += 3;
        clen -= 4;
        scnt += 3;
        if (scnt > slen) {
            return 0;
        }
    }
    if (clen <= 0) {
        *q = '\0';
        return scnt;
    }
    if (base64_equal_sign(p) == 1) {
        /* 有一个等号 */
        temp1b = *(p++);
        temp2b = *(p++);
        temp3b = *(p++);
        /* 查表 */
        temp1b = esab[(int)temp1b];
        temp2b = esab[(int)temp2b];
        temp3b = esab[(int)temp3b];
        q[0] = (temp1b << 2) | (temp2b >> 4);
        q[1] = (temp2b << 4) | (temp3b >> 2);
        q += 2;
        scnt += 2;
        if (scnt > slen) {
            return 0;
        }
    }
    if (base64_equal_sign(p) == 2) {
        /* 有两个等号 */
        temp1b = *(p++);
        temp2b = *(p++);
        /* 查表 */
        temp1b = esab[(int)temp1b];
        temp2b = esab[(int)temp2b];
        q[0] = (temp1b << 2) | (temp2b >> 4);
        q += 1;
        scnt += 1;
        if (scnt > slen) {
            return 0;
        }
    }
    *q = '\0';
    return scnt;
}

/* 返回解码了几个字节，剩下的再clen传出参数中 */
/* 输出还剩几个字节没有解码，base64算法只会剩下0-3个 */
/* 为减轻复杂度，输出空间的长度假定为足够大，实际上解码时输出空间一定小于输出空间 */
int base64_decode_stream(char *code, int *clen, void *str)
{
    int scnt = 0;                           /* str解码后的长度 */
    char *p = code;                         /* 始终指向未解码的部分 */
    unsigned char *q = (unsigned char*)str; /* 始终指向传入地址空间的 */
    register unsigned char temp1b = 0;
    register unsigned char temp2b = 0;
    register unsigned char temp3b = 0;
    register unsigned char temp4b = 0;

    if (*clen < 4) {
        return 0;
    }
    scnt = 0;
    while (*clen >= 4 && base64_equal_sign(p) == 0) {
        /* 跳过\r\n，\r\n只会出现在4的倍数+1的位置(77, 78) */
        if (*p == '\r') {
            p += 2;
            *clen -= 2;
            continue;
        }
        temp1b = *(p++);
        temp2b = *(p++);
        temp3b = *(p++);
        temp4b = *(p++);
        /* 查表 */
        temp1b = esab[(int)temp1b];
        temp2b = esab[(int)temp2b];
        temp3b = esab[(int)temp3b];
        temp4b = esab[(int)temp4b];
        q[0] = (temp1b << 2) | (temp2b >> 4);
        q[1] = (temp2b << 4) | (temp3b >> 2);
        q[2] = (temp3b << 6) | temp4b;
        q += 3;
        *clen -= 4;
        scnt += 3;
    }
    if (*clen < 4) {
        return scnt;
    }
    if (base64_equal_sign(p) == 1) {
        /* 有一个等号 */
        temp1b = *(p++);
        temp2b = *(p++);
        temp3b = *(p++);
        /* 查表 */
        temp1b = esab[(int)temp1b];
        temp2b = esab[(int)temp2b];
        temp3b = esab[(int)temp3b];
        q[0] = (temp1b << 2) | (temp2b >> 4);
        q[1] = (temp2b << 4) | (temp3b >> 2);
        scnt += 2;
        *clen -= 4;
    }

    if (base64_equal_sign(p) == 2) {
        /* 有两个等号 */
        temp1b = *(p++);
        temp2b = *(p++);
        /* 查表 */
        temp1b = esab[(int)temp1b];
        temp2b = esab[(int)temp2b];
        q[0] = (temp1b << 2) | (temp2b >> 4);
        scnt += 1;
        *clen -= 4;
    }
    return scnt;
}

/**
 * 仅仅供base64_decode使用
 * @param 参数是仅有4个字符的字符串, 不会判断\0
 * @return 返回=号的个数
 */
inline static int
base64_equal_sign(char *ch4)
{
    register int i;
    int n = 0;

    for (i = 0; i < 4; i++) {
        if (ch4[i] == '=') {
            n++;
        }
    }
    return n;
}
