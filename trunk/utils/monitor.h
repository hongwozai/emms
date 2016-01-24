/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：monitor.h
 ** 创建人：路泽亚
 ** 描  述：系统状态监控函数，cpu，内存，硬盘
 ** 注  意：1.
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-14
 ********************************************************************/
#ifndef MY_UTILS_MONITOR_H
#define MY_UTILS_MONITOR_H


/**
 * 获得总cpu使用率
 * @return 返回-1 打开文件失败
 *         返回-2 参数传入错误
 *         返回大于0的数字，正常
 */
extern double get_cpu();

/**
 * 获得内存使用率
 * @param total 必须传递内存，内存总数，单位kB
 * @return 返回-1 打开文件失败
 *         返回-2 参数传入错误
 *         返回大于0的数字，正常
 */
extern double get_mem(int *total);

/**
 * 获得系统运行状态
 * 两个参数都必须传递内存
 * @param all 两个参数单位均为kB
 * @param use
 * @return 返回-1 打开文件失败
 *         返回-2 参数传入错误
 *         返回大于0的数字，正常
 */
extern double get_disk(int *all, int *use);

#endif /* MY_UTILS_MONITOR_H */
