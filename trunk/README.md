EMMS项目
========

项目内容简介
------------

   本系统能够对进出局域网的电子邮件进行捕获与分析，对敏感邮件进行内容监控并对垃圾邮件进行有效识别，具体功能如下：
1）实现对SMTP协议、POP3协议、IMAP协议以及WEBMAIL的解析与还原；对邮件地址关键字、主题关键字和邮件正文内容关键字、附件名关键字以及文本文件附件内容关键字的监测与封堵，并记录监测日志和封堵日志；对垃圾邮件的监测，并记录监测日志。
2）系统前台具备完善的管理功能，如配置功能、查询功能、备份功能等。

项目开发人员
------------

哈尔滨工程大学信息安全研究中心
培训第二组：组长路泽亚，组员：李珍，王新，孙亮亮

项目目录结构
------------

基本目录文件结构如下：
  * bin               项目可执行文件的生成路径
  * inc               项目头文件目录
  * src               项目源文件目录
  * test              单元测试代码及测试生成文件的路径
  * utils             项目公共部分代码（数据结构等）
  * tools             项目中使用的工具脚本，测试脚本等
  * front             项目的前台部分
  * Makefile          整个项目的makefile
  * README.md         项目的简介，即本文档
  * .dir-locals.el    emacs目录变量设置文件

源文件目录下的结构（在src与utils目录中）：
  * deps     使用gcc -MM生成的依赖关系文件的目录
  * objs     编译生成的中间文件

测试目录文件结构：
  * test.c    测试文件的源代码
  * libtest.a 测试生成的库文件
  * emms-test 测试可执行文件

可执行文件目录：
  * emms      生成的最终可执行文件

工具文件目录：
  * transport.py 远程传送文件夹的工具，需安装python与paramiko包
  * client.py 测试主控模块链接的客户端程序

模块文件规划
------------

主控模块：
  * inc/global.h
  * src/global.c
  * src/main.c

匹配模块：
  * inc/match.h
  * src/match.c

监测模块：
  * inc/monitor.h
  * src/monitor.c

垃圾邮件模块：
  * inc/spam.h
  * src/spam.c

封堵模块：
  * inc/kill.h
  * src/kill.c

日志模块：
  * inc/db.h
  * src/db.c
  * inc/log.h
  * src/log.c

解析还原模块：
  * inc/analy.h
  * inc/analy_noweb.h
  * inc/analy_webmail.h
  * src/analy.c
  * src/analy_noweb.c
  * src/analy_webmail.c
