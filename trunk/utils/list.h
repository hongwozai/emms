/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：list.h
 ** 创建人：路泽亚
 ** 描  述：实现链表, 链式队列(链表), 链式堆栈（链表），主要当作堆栈和队列使用
 **
 ** 当前版本： v1.0
 ** 作   者：路泽亚
 ** 完成日期： 2015-12-27
 ** 更改日志：1.内存改为直接分配，不使用空指针直接指向，可以减少分配次数。但是
 **           不要保存大块的数据，仅保存其指针即可,这其中应该直接使用结构体
 **         2.暂时只用做堆栈，下一步是加入迭代方式，可以彻底的隔离结构与数据
 **         3.鉴于效率，对参数只进行断言判断
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-10-16
 ** 注  意：1. 链表长度使用int实现，支持2^31个节点
 **        2. 仅使用list_pop,list_push就是链式栈
 **        3. 仅使用list_push,list_popleft就是链式队列
 **        4. 该实现做的判断过多，为了实现通用，
 **           效率略低，可以在不在意效率的地方使用
 **        5. 这个实现使用malloc分配内存，而且每个节点都要自己分配内存，
 **           并不是一种很好的方式
 ********************************************************************/
#ifndef LIST_H
#define LIST_H

#define T List_T

typedef struct list List_T;

extern T*   List_new(int size);
extern void List_free(T *list);
extern int  List_length(T *list);
extern int  List_empty(T *list);
extern int  List_append(T *list1, T *list2);
extern int  List_add(T *list, void *data, int index);
extern int  List_del(T *list, int index);
extern int  List_pop(T *list, void *data);
extern int  List_push(T *list, void *data);
extern int  List_popleft(T *list, void *data);

#undef T
#endif /* LIST_H */
