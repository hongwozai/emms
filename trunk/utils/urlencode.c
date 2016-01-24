/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：urlencode.c
 ** 创建人：路泽亚
 ** 描  述：url编码方式的解码
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-01
 ********************************************************************/
#include "match.h"
#include "base64.h"
#include "urlencode.h"

#include <string.h>

inline static unsigned char
url_decode_byte(char *c)
{
    unsigned char res;
    register unsigned char a;
    register unsigned char b;
    a = (c[0] >= 65)? (c[0] - 55): (c[0] - 48);
    b = (c[1] >= 65)? (c[1] - 55): (c[1] - 48);
    res = ( a << 4 ) | ( b & 0x0F );
    return res;
}

/**
 * 遇到+号输出空格
 * 遇到%号，去掉%号并将后面的两个字符转换为一个字符
 */
int url_decode(char *str, int slen, char *buf)
{
    int i, j;
    register unsigned char tmp;

    if (!str || slen <= 0 || !buf ) {
        return 0;
    }
    for (i = 0, j = 0; i < slen; i++) {
        if (str[i] == '+') {
            buf[j++] = ' ';
        } else if (str[i] == '%') {
            tmp = url_decode_byte(&str[i+1]);
            buf[j++] = tmp;
            i += 2;
        } else if (str[i] == '\0'){
            break;
        } else {
            buf[j++] = str[i];
        }
    }
    buf[j] = '\0';
    return j;
}

/**
 * 直接替换原来的url，并在其后加上\0
 * @param str
 * @param slen
 */
int url_decode_replace(char *str, int slen)
{
    int i, j;
    unsigned char tmp;

    if (!str || slen <= 0) {
        return 0;
    }
    for (i = 0, j = 0; i < slen; i++) {
        if (str[i] == '+') {
            str[j++] = ' ';
        } else if (str[i] == '%') {
            if (i + 1 == slen) {
                break;
            }
            if (i + 2 == slen) {
                break;
            }
            tmp = url_decode_byte(&str[i+1]);
            str[j++] = tmp;
            i += 2;
        } else if (str[i] == '\0') {
            break;
        } else {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
    return j;
}

/**
 * 会将无法解码的部分复制到目的字符串上
 * @param str
 * @param slen
 */
int url_decode_replace_2(char *str, int slen, int *reallen)
{
    int i, j, k = 0;
    unsigned char tmp;

    if (!str || slen <= 0) {
        return 0;
    }
    for (i = 0, j = 0; i < slen; i++) {
        if (str[i] == '+') {
            str[j++] = ' ';
        } else if (str[i] == '%') {
            if (i + 1 == slen) {
                k = j;
                str[j++] = '%';
                break;
            }
            if (i + 2 == slen) {
                k = j;
                str[j++] = '%';
                str[j++] = str[i + 1];
                break;
            }
            tmp = url_decode_byte(&str[i+1]);
            str[j++] = tmp;
            i += 2;
        } else if (str[i] == '\0') {
            break;
        } else {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
    *reallen = j;
    return k? k : j;
}

/* 等号没有处理 */
int quote_decode(char *str, int slen, char *buf)
{
    int i, j;
    register unsigned char tmp;

    if (!str || slen <= 0 || !buf ) {
        return 0;
    }
    for (i = 0, j = 0; i < slen; i++) {
        if (str[i] == '=') {
            tmp = url_decode_byte(&str[i+1]);
            buf[j++] = tmp;
            i += 2;
        } else if (str[i] == '\0'){
            break;
        } else {
            buf[j++] = str[i];
        }
    }
    buf[j] = '\0';
    return j;
}

void str_decode(char *raw, char *str)
{
    int len;
    char *p = raw, *q;

    if (*(p++) != '=' && *(p++) != '=') {
        strcpy(str, raw);
        url_decode_replace(str, strlen(str));
        return;
    }
    p += 1;
    p = strchr(p, '?');
    /* quote的 */
    if (*(p + 1) == 'Q') {
        p += 3;
        strcpychr(p, str, '?');
        q = strchr(p, '?');
        len = quote_decode(p, (int)(q - p), str);
        strcpy(str + len, q + 2);
        return;
    }
    if (*(p + 1) != 'B') {
        strcpy(str, raw);
        return;
    }
    /* base64的 */
    p += 3;
    strcpychr(p, str, '?');
    q = strchr(p, '?');
    len = base64_decode(p, (int)(q - p), str, (int)(q - p));
    strcpy(str + len, q + 2);
}
