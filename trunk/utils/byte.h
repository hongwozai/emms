/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：byte.h
 ** 创建人：路泽亚
 ** 描  述：一些共用的位运算
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-11-24
 ********************************************************************/
#ifndef BYTE_H
#define BYTE_H

/* 判断是否为2的幂次 */
/* 当2的幂次时，返回0, 其余返回非0 */
#define IS_POW_OF_TWO(x) ((x) & ((x) - 1))

/* 求模运算 */
#define MOD(x, y) ((x) & ((y) - 1))

/* 过程与求模类似，该算法只对d为2的倍数时有用 */
#define IS_MULTIPLE(x, d)                       \
    (((x) & ((d) - 1)) == 0)

#define IS_MULTIPLE_OF_ADDR(x)                  \
    IS_MULTIPLE(x, sizeof(void*))

#define ROUND_UP(l, d)                          \
    (((l) | ((d) - 1)) + 1)

#define ROUND_UP_OF_ADDR(l)                     \
    ROUND_UP(l, sizeof(void*))

/**
 * 将参数园整为2的幂次
 * @param int 传入的数值
 * @return 返回大于传入参数的最小的2的幂次
 */
extern int roundup_pow_of_two(int);

#endif /* BYTE_H */
