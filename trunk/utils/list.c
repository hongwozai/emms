/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：list.c
 ** 创建人：路泽亚
 ** 描  述：链表的实现
 ** TODO: 1. 实现对list数据抽象，可以仅使用函数和宏增加操作函数(但是要考虑效率)
 **       2. list_index
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-10-16
 ********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mem.h"
#include "list.h"

#define T struct list
#define HEAD_T struct list_head

struct list_head {
    struct list_head *next;
};

struct list {
    int size;
    int length;
    struct list_head *head;
    struct list_head *tail;
};


T* List_new(int size)
{
    T *list;

    if (!NEW0(list))
        return NULL;
    list->size   = size;
    list->length = 0;
    list->head   = NULL;
    list->tail   = NULL;
    return list;
}

void List_free(T *list)
{
    HEAD_T *tmp;

    assert(list);

    /* 先释放list所有的节点,再释放list本身 */
    /* 要避免在for循环中写入太多语句，尤其是有先后顺序的，要避免副作用，以免引入
     * 难以察觉的bug */
    for (tmp = list->head; list->length != 0; list->length--) {
        tmp = list->head;
        list->head = list->head->next;
        FREE(tmp);
    }
    FREE(list);
}

int List_length(T *list)
{
    assert(list);
    return list->length;
}

int List_empty(T *list)
{
    assert(list);
    return list->length? 0 : 1;
}

int List_append(T *list1, T *list2)
{
    assert(list1);
    assert(list2);
    if (list1->length == 0) {
        list1->head   = list2->head;
        list1->tail   = list2->tail;
        list1->length = list2->length;
        return list1->length;
    }
    list1->tail->next = list2->head;
    list1->tail       = list2->tail;
    list1->length     = list1->length + list2->length;
    FREE(list2);
    return list1->length;
}

int List_add(T *list, void *data, int index)
{
    HEAD_T *tmp, *p;

    assert(list);
    assert(data);
    assert(index > 0);

    if (!ALLOC(tmp, list->size + sizeof(HEAD_T)))
        return -1;
    tmp->next = NULL;
    memcpy((char*)tmp + sizeof(HEAD_T), data, list->size);
    /* 如果要添加到头部 */
    if (index == 0) {
        tmp->next  = list->head;
        list->head = tmp;
        /* 如果是空表，就将tail也指向该节点 */
        if (list->length == 0) {
            list->tail = list->head;
        }
        goto success;
    }
    /* 如果是最后一个节点，那么更改tail
     * 为了效率，不使用list_push函数*/
    if (index == list->length) {
        list->tail->next = tmp;
        list->tail       = tmp;
        goto success;
    }
    /* 找到index的节点 */
    for (p = list->head; index != 1; index--, p = p->next);
    tmp->next = p->next;
    p->next   = tmp;
success:
    return ++list->length;
}

int List_del(T *list, int index)
{
    int indextmp = index;
    HEAD_T *p, *tmp;

    assert(list);
    assert(index > 0);
    /**
     * 判断长度是否为1，为1(index也肯定为1)，结果为空表
     * 判断index是否为1（此时长度不为1），需要更改head指针
     * 判断index==list->length（此时均不为1），需要更改tail指针
     */
    if (list->length == 1) {
        /* index必定为1 */
        FREE(list->head);
        list->head = NULL;
        list->tail = NULL;
        return --list->length;
    }
    if (index == 1) {
        p = list->head;
        list->head = list->head->next;
        FREE(p);
        return --list->length;
    }
    for (p = list->head; indextmp != 2; p = p->next, indextmp--);
    tmp = p->next;
    p->next = p->next->next;
    FREE(tmp);
    /* 如果是最后一个节点，还要tail指针指向最后一个节点,不是则不用 */
    /* 使用next等于NULL来判断是否是最后一个节点 */
    /* if (index == list->length) { */
    list->tail = p->next?p->next:p;
    /* } */
    return --list->length;
}

int List_pop(T *list, void *data)
{
    int index;
    HEAD_T *p;

    assert(list);
    assert(data);
    if (list->length == 0) {
        return -1;
    }
    /* 如果只有一个，则变成空表 */
    if (list->length == 1) {
        list->length = 0;
        memcpy(data, (char*)list->head + sizeof(HEAD_T), list->size);
        FREE(list->head);
        list->head   = NULL;
        list->tail   = NULL;
        return 0;
    }
    /* 如果有多个 */
    for (index = list->length, p = list->head;
         index != 2;
         p = p->next, index--);
    p->next = NULL;
    memcpy(data, (char*)list->tail + sizeof(HEAD_T), list->size);
    FREE(list->tail);
    list->tail = p;
    return --list->length;
}

int List_push(T *list, void *data)
{
    HEAD_T *p;

    assert(list);
    assert(data);
    if (!ALLOC(p, list->size + sizeof(HEAD_T))) {
        return -1;
    }
    memcpy((char*)p + sizeof(HEAD_T), data, list->size);
    p->next = NULL;
    /* 如果是空表 */
    if (list->length == 0) {
        list->head = p;
    } else {
        list->tail->next = p;
    }
    list->tail = p;
    return ++list->length;
}

int List_popleft(T *list, void *data)
{
    HEAD_T *tmp;

    assert(list);
    assert(data);
    if (list->length == 0)
        return -1;
    if (list->length == 1) {
        memcpy(data, (char*)list->head + sizeof(HEAD_T), list->size);
        FREE(list->head);
        list->head = NULL;
        list->tail = NULL;
        return --list->length;
    }
    tmp = list->head;
    list->head = list->head->next;
    memcpy(data, (char*)tmp + sizeof(HEAD_T), list->size);
    FREE(tmp);
    return --list->length;
}
