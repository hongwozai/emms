/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：cfg.h
 ** 创建人：路泽亚
 ** 描  述：ini格式配置文件读取
 ** 注  意：这个配置文件读取，是进行简单的词法分析与语法分析
 **        但是之前写的风格不统一，而且还有很多问题，还待重写
 **        本配置文件读取直接使用语法树和链表，所以效率并不高
 **        而且本程序对错误处理并不好，过于简单
 **
 ** 当前版本：v1.0
 ** 作   者：路泽亚
 ** 版本改变：去掉依赖，更改一些名称与格式
 ** 完成日期：2015-11-27
 **
 ** 当前版本：v0.9
 ** 作   者：路泽亚
 ** 完成日期：2015-07-23
 ********************************************************************/
#ifndef PUBLIC_CFG_H_INCLUDE
#define PUBLIC_CFG_H_INCLUDE

/* 配置文件的处理中，如果内存分配失败就直接退出程序 */

/* ==================== 临时取值使用 ============================= */
/**
 * 临时取值使用，若是多次读取请使用打开关闭的方式
 * 打开配置文件，读取，并关闭
 * @param file 配置文件名
 * @param section 节名
 * @param key 键名
 * @param value 输出参数,输出整数型
 * @return 返回成功与否，0代表成功，非0代表失败（返回-1代表没有该键值对）
 */
extern int cfg_readint(const char *file, const char *section,
                       const char *key, int *value);

/**
 * 同cfg_readint，只是输出的是字符串
 * @param file
 * @param section
 * @param key
 * @param value 输出参数,直接传字符串
 * @return 同cfg_readint
 */
extern int cfg_readstring(const char *file, const char *section,
                          const char *key, char *value);


/* ========================== 多次取值使用 ======================= */

typedef struct Cfg_tag cfg_t;

/**
 * 打开配置文件,读取配置文件中的信息,并不关闭配置文件
 * @param cfg
 * @param file
 */
extern void cfg_open(cfg_t **cfg, const char *file);

/**
 * 关闭配置文件
 * @param cfg
 */
extern void cfg_close(cfg_t *cfg);

/**
 * 读取配置结构体中保存的信息，并不进行IO操作
 * @param cfg
 * @param section
 * @param key
 * @param value 传出参数，直接传回字符串，整形需要转换
 * @return 返回读取成功与否，返回0成功，返回非0(-1)失败
 */
extern int cfg_read(cfg_t *cfg, const char *section, const char *key,
                    char *value);

#endif /* PUBLIC_CFG_H_INCLUDE */
