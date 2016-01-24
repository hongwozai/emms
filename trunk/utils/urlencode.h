/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：urlencode.h
 ** 创建人：路泽亚
 ** 描  述：urlencode的解码，quote printed解码
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-01
 ********************************************************************/
#ifndef URLENCODE_H
#define URLENCODE_H

/**
 * urlencode解码，注意buf与str必须不能重叠
 * 而且buf缓冲区必须分配，缓冲区长度不超过str的长度
 * @param str 编码的字符串
 * @param slen 编码字符串的长度
 * @param buf  编码后的字符串
 * @return 返回编码后的字符串长度，该长度不包括\0
 *         返回0代表失败
 */
extern int url_decode(char *str, int slen, char *buf);
extern int quote_decode(char *str, int slen, char *buf);
extern void str_decode(char *raw, char *str);

/**
 * 这是不用缓冲区的版本，快不了多少，但是不用分配空间
 * @param str
 * @param slen
 * @return 返回编码后的字符串的长度，该长度不包括\0
 *         返回0代表失败
 */
extern int url_decode_replace(char *str, int slen);

/**
 * 不用缓冲区，并对没有解析完的部分进行处理（复制到目标串最后并返回它的地址）
 * @param str
 * @param slen
 * @param reallen 传出参数，返回目标串的真正长度
 * @return 返回编码后的长度（不算未解码的部分）
 */
extern int url_decode_replace_2(char *str, int slen, int *reallen);

#endif /* URLENCODE_H */
