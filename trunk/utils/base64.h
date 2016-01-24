/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：base64.h
 ** 创建人：路泽亚
 ** 描  述：base64编码解码的头文件
 **        rfc规定base64中一行不可以超过76个字符，超过就添加回车换行\r\n
 **
 ** 注  意：1.注意接口使用时的编码，本实现不进行编码转换
 **        2.本实现并不分配内存，内存自行分配
 **        3.使用int表示长度，支持2^31(2G长)字符串长度
 **        4.传入传出的地址空间不能重合
 **        5.该实现支持二进制的编码解码（字符串中间允许空洞）
 **        6.base64_decode, base64_encode不支持中间有回车换行
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-10-03
 ********************************************************************/
#ifndef BASE64_H
#define BASE64_H

/**
 * base64 编码函数
 * @param str  要编码的字符串, 可以为二进制类型
 * @param slen str的长度，str并不依赖于结尾的\0，而是使用slen来判断str是否结尾
 * @param code 编码后的字符串，char类型, 会在结尾加上\0
 *             函数本身并不分配空间,需要预先分配好空间,
 *             注意编码后的空间会比之前的大至少三分之一,
 *             并且不要将str重新传入code，会导致函数出现错误
 * @param clen 传入空间长度
 * @return 返回非0成功，代表编码后的字节数，返回0，代表编码失败
 *         clen == 0, str,code == NULL都会导致编码失败
 */
extern int base64_encode(void *str, int slen, char *code, int clen);

/**
 * base64 解码函数
 * @param code 要解码的字符串, 注意应该为char类型
 * @param clen code的长度，不使用结尾的\0
 * @param str  解码后的字符串, 函数并不分配空间
 * @param slen 传入的空间长度，防止缓冲区溢出
 * @return 返回非0，解码成功, 代表解码后的字符长度，返回0，代表解码失败
 *         clen == 0, str,code == NULL都会导致解码失败
 */
extern int base64_decode(char *code, int clen, void *str, int slen);

/**
 * base64解码函数，可以用于流式解析，不会向结尾插入\0
 * @param code  输入缓冲
 * @param clen  传出参数，输入缓冲的字节数，传出还剩多少字节未解析（0-3）
 * @param str   要输出的缓冲区空间，默认输出缓冲足够大
 * @return 返回已经解析的字节数
 */
extern int base64_decode_stream(char *code, int *clen, void *str);

#endif /* BASE64_H */
