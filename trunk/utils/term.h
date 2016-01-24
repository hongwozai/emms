/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：term.h
 ** 创建人：路泽亚
 ** 描  述：有关于终端的一些函数，宏
 ** 注  意：1.正确使用颜色宏
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-10-06
 ********************************************************************/
#ifndef TERM_H
#define TERM_H

/* special effect */
#define UNDERLINE(string)      "\e[4m"string"\e[0m"

/* foreground */
#define BLACK_STR(string)      "\e[30m"string"\e[0m"
#define RED_STR(string)        "\e[31m"string"\e[0m"
#define GREEN_STR(string)      "\e[32m"string"\e[0m"
#define BROWN_STR(string)      "\e[33m"string"\e[0m"
#define BLUE_STR(string)       "\e[34m"string"\e[0m"
#define MAGENTA_STR(string)    "\e[35m"string"\e[0m"
#define CYAN_STR(string)       "\e[36m"string"\e[0m"
#define WHITE_STR(string)      "\e[37m"string"\e[0m"

/* background */
#define BLACK_BG(string)      "\e[40m"string"\e[0m"
#define RED_BG(string)        "\e[41m"string"\e[0m"
#define GREEN_BG(string)      "\e[42m"string"\e[0m"
#define BROWN_BG(string)      "\e[43m"string"\e[0m"
#define BLUE_BG(string)       "\e[44m"string"\e[0m"
#define MAGENTA_BG(string)    "\e[45m"string"\e[0m"
#define CYAN_BG(string)       "\e[46m"string"\e[0m"
#define WHITE_BG(string)      "\e[47m"string"\e[0m"

#endif /* TERM_H */
