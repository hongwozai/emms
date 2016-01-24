#include <errno.h>
#include <stdio.h>
#include <iconv.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "inc/global.h"
#include "utils/fileutils.h"
#include "utils/tpool.h"
#include "utils/shash.h"
#include "inc/spam.h"
#include "inc/analy_noweb.h"
#include "inc/db.h"
#include "inc/log.h"
#include "inc/kill.h"
#include "inc/analy.h"
#include "inc/match.h"
#include "utils/mcq.h"
#include "utils/mem.h"
#include "utils/list.h"
#include "utils/byte.h"
#include "inc/global.h"
#include "utils/tpool.h"
#include "utils/debug.h"
#include "inc/monitor.h"
#include "utils/debug.h"
#include "utils/match.h"
#include "utils/mhash.h"
#include "utils/unitest.h"
#include "utils/urlencode.h"
#include "utils/base64.h"

TEST(base64)
{
    char *p = "ZnJvbTpuaW5pNzQ5ODxuaW5pNzQ5OEAxNjMuY29tPui3r+azveS6mg0KdG86bmluaTc0OTg8\r\nbmluaTc0OThAMTYzLmNvbT4NCnN1YmplY3Q6dGhlDQpjb250ZW50OnRlcm0NCglmb";
    int plen = strlen(p);
    printf("plen: %d\n", plen);
    char buf[1024];
    int ret;

    ret = base64_decode_stream(p, &plen, buf);
    ASSERT_EQ(plen, 1);
    buf[ret] = '\0';
    printf("plen: %d\nstr: %s\n", plen, buf);

}

TEST(byte)
{
    ASSERT_EQ(roundup_pow_of_two(5), 8);
    ASSERT_EQ(roundup_pow_of_two(9), 16);
    ASSERT_EQ(roundup_pow_of_two(16), 32);
    ASSERT_EQ(roundup_pow_of_two(0), 1);

    ASSERT_EQ(IS_POW_OF_TWO(1), 0);
    ASSERT_NEQ(IS_POW_OF_TWO(3), 0);
    ASSERT_EQ(IS_POW_OF_TWO(0), 0);
    ASSERT_EQ(IS_POW_OF_TWO(16), 0);

    int i;
    for (i = 0; i < 25; i++) {
        ASSERT_EQ(IS_MULTIPLE(i, 4), i%4 == 0);
    }

    ASSERT_EQ(MOD(3, 2), 1);
    ASSERT_EQ(MOD(1, 4), 1);
    ASSERT_EQ(MOD(9, 4), 1);
    ASSERT_EQ(MOD(15, 4), 3);

    ASSERT_EQ(char16toint("123", 3), 291);
    ASSERT_EQ(char16toint("3a1", 3), 929);
    ASSERT_EQ(char16toint("3aa", 3), 938);
}

TEST(url)
{
    char *abc = "ascii%e7abc%a0%81%e8%a1%a8";
    char *abcd = "ascii%e8%a1%a8abc%a8%a8wer%a8";
    char *abcde = "%3C%3Fxml%20version%3D%221.0%22%3F%3E%3Cobject%3E%3Cstring%20name%3D%22id%22%3Ec%3A1449705800465%3C%2Fstring%3E%3Cobject%20name%3D%22attrs%22%3E%3Cstring%20name%3D%22account%22%3E%22123%22%26lt%3Bhongwozai%40163.com%26gt%3B%3C%2Fstring%3E%3Cboolean%20name%3D%22showOneRcpt%22%3Efalse%3C%2Fboolean%3E%3Carray%20name%3D%22to%22%3E%3Cstring%3E%22%E6%88%91%22%26lt%3Bhongwozai%40163.com%26gt%3B%3C%2Fstring%3E%3C%2Farray%3E%3Carray%20name%3D%22cc%22%2F%3E%3Carray%20name%3D%22bcc%22%2F%3E%3Cstring%20name%3D%22subject%22%3Eshit%3C%2Fstring%3E%3Cboolean%20name%3D%22isHtml%22%3Etrue%3C%2Fboolean%3E%3Cstring%20name%3D%22content%22%3E%26lt%3Bdiv%20style%3D%22line-height%3A1.7%3Bcolor%3A%23000000%3Bfont-size%3A14px%3Bfont-family%3AArial%22%26gt%3Bshit%26lt%3B%2Fdiv%26gt%3B%3C%2Fstring%3E%3Cint%20name%3D%22priority%22%3E3%3C%2Fint%3E%3Cboolean%20name%3D%22saveSentCopy%22%3Etrue%3C%2Fboolean%3E%3Cstring%20name%3D%22charset%22%3EGBK%3C%2Fstring%3E%3C%2Fobject%3E%3Cboolean%20name%3D%22returnInfo%22%3Efalse%3C%2Fboolean%3E%3Cstring%20name%3D%22";
    char buf[2048];

    ASSERT_EQ(url_decode("123", strlen("123"), buf), 3);
    ASSERT_EQ(url_decode(abc, strlen(abc), buf), 14);
    printf("buf: %s\n", buf);

    strcpy(buf, abcd);
    ASSERT_EQ(url_decode_replace(buf, strlen(buf)), 17);
    printf("buf: %s\n", buf);

    strcpy(buf, abcde);
    ASSERT_EQ(url_decode_replace(buf, strlen(buf)), 680);
    printf("buf: %s\n", buf);

    int size;
    strcpy(buf, "%3C%3Fxml%20version%3D%221.0%22");
    ASSERT_EQ(url_decode_replace_2(buf, strlen(buf), &size), 19);
    printf("buf: %s\n", buf);

    strcpy(buf, "%3C%3Fxml%20version%3D%221.0%");
    ASSERT_EQ(url_decode_replace_2(buf, strlen(buf), &size), 18);
    ASSERT_EQ(size, 19);
    printf("buf: %s\n", buf);

    strcpy(buf, "%3C%3Fxml%20version%3D%221.0%2");
    ASSERT_EQ(url_decode_replace_2(buf, strlen(buf), &size), 18);
    ASSERT_EQ(size, 20);
    printf("buf: %s\n", buf);
}

struct mcq_node {
    char *data;
};

TEST(mcq)
{
    Mcq_T *q;
    struct mcq_node a, b;

    a.data = "123";
    b.data = "456";
    q = Mcq_new(sizeof(struct mcq_node), 4);

    ASSERT_NEQ(q, NULL);
    ASSERT_EQ(Mcq_enq(q, &a), 0);
    ASSERT_EQ(Mcq_enq(q, &a), 0);
    ASSERT_EQ(Mcq_enq(q, &b), 0);
    ASSERT_EQ(Mcq_enq(q, &b), -1);
    ASSERT_EQ(Mcq_deq(q, &b), 0);
    ASSERT_STR_EQ(b.data, "123");
    ASSERT_EQ(Mcq_deq(q, &b), 0);
    ASSERT_STR_EQ(b.data, "123");
    ASSERT_EQ(Mcq_deq(q, &a), 0);
    ASSERT_STR_EQ(a.data, "456");
    ASSERT_EQ(Mcq_deq(q, &b), -1);
    Mcq_free(q);
}

TEST(mpool)
{
    int i;
    char *a, *b, *c;
    struct timespec start, end;
    Mpool_T *m = Mpool_new(8, 10);
    ASSERT_NEQ(m, NULL);

    for (i = 0; i < 500; i++) {
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        a = Mpool_alloc(m);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
        printf("mpool nsec: %ld\n", end.tv_nsec - start.tv_nsec);
    }

    b = Mpool_alloc(m);
    c = Mpool_alloc(m);
    Mpool_free(m, a);
    Mpool_free(m, b);
    Mpool_free(m, c);
    Mpool_dispose(m);
}

struct test_hash {
    HASH_HEAD;
    char *data;
};

TEST(mhash)
{
    struct tuple4 key = {1, 1, 1, 1};
    struct tuple4 key1 = {2, 1, 1, 1};
    struct test_hash *tmp;
    char buf[1500], origin[1500];
    memset(origin, 0, 1500);
    Mpool_T *p = Mpool_new(sizeof(struct test_hash), 128);
    Mhash_T *m = Mhash_new(10, sizeof(struct test_hash), p);
    tmp = Mhash_insert(m, &key);
    tmp->data = "hello world!";
    tmp = Mhash_insert(m, &key);
    tmp->data = "world hello!";
    tmp = Mhash_insert(m, &key1);
    tmp->data = "123";
    int i;
    for (i = 0; i < 100; i++) {
        BENCH("find: ");
        tmp = Mhash_find(m, &key1);
        END;
        BENCH("cpy");
        memcpy(buf, origin, 1500);
        END;
    }
    tmp = Mhash_find(m, &key1);
    if (tmp) {
        ASSERT_EQ(tmp->data, "123");
    } else {
        ASSERT_EQ(m, NULL);
    }
    ASSERT_EQ(Mhash_del(m, &key), 0);
    ASSERT_EQ(Mhash_del(m, &key), 0);
    ASSERT_EQ(Mhash_del(m, &key1), 0);
    ASSERT_EQ(Mhash_del(m, &key1), -1);
    Mhash_free(m);
    Mpool_dispose(p);
}

TEST(str_match)
{
    ASSERT_EQ(istxt("12343234543.docx"), 0);
    ASSERT_EQ(istxt("12343234543.txt"), 1);
}

TEST(kmp_match)
{
    const char *a = kmp_match("23451236789", "123", '9');
    ASSERT_NEQ_RET(a, NULL);
    printf("%p\n", a);
}

TEST(ac_match)
{
    am_t *a1 = am_new(2);
    ASSERT_EQ(am_addword(a1, "123"), 1);
    ASSERT_EQ(am_addword(a1, "124"), 2);
    ASSERT_EQ(am_addword(a1, "4567"), 3);
    ASSERT_EQ(am_addword(a1, "1234"), 4);

    ASSERT_EQ(am_build(a1), 0);

    ASSERT_STR_EQ(am_match(a1, "123456"), "123");
    ASSERT_STR_EQ(am_match(a1, "234567"), "4567");
    ASSERT_EQ(am_match(a1, "abcdefg"), NULL);
    ASSERT_STR_EQ(am_match(a1, "1234567"), "123");

    ASSERT_EQ(am_rebuild(a1, 2), 0);

    ASSERT_EQ(am_addword(a1, "路泽亚"), 1);
    ASSERT_EQ(am_addword(a1, "李珍"), 2);
    ASSERT_EQ(am_addword(a1, "孙亮亮"), 3);
    ASSERT_EQ(am_addword(a1, "路泽亚"), 4);
    am_build(a1);

    ASSERT_STR_EQ(am_match(a1, "asdfasdf阿斯顿发送路泽亚"), "路泽亚");
    ASSERT_EQ(am_match(a1, "bcefcde"), NULL);
    am_destroy(a1);

    am_t *a2 = am_new(4);
    am_addword(a2, "qq");
    am_addword(a2, "fuck");
    am_addword(a2, "shit");
    am_build(a2);
    ASSERT_STR_EQ(am_match(a2, "fuck"), "fuck");
    am_destroy(a2);
}

void thead_func(void *data)
{
    int i = (int)(long)data;
    printf("thread %ld finished %d\n", pthread_self(),i);
}

void thead_func1(void *data)
{
    (void)data;
    for (;;) {
        sleep(1);
    }
}

TEST(tpool)
{
    int i;
    Tpool_T *pool = Tpool_new(10);
    Tpool_run(pool);
    Tpool_addwork(pool, thead_func1, NULL);
    for (i = 0; i < 1000; i++) {
        Tpool_addwork(pool, thead_func, (void*)(long)i);
    }
    Tpool_free(pool);
}

/* TEST(analy) */
/* { */
/*     db_init("192.168.100.34/LINUXORCL", "SCOTT", "123"); */
/*     kill_init(); */
/*     monitor_init(); */
/*     log_init(); */
/*     match_init(); */
/*     analy_init("eth0"); */

/*     analy_run(); */
/*     monitor_run(); */
/*     log_run(); */
/*     pause(); */

/*     monitor_stop(); */
/*     log_stop(); */
/*     kill_close(); */
/*     match_close(); */
/*     db_close(); */
/* } */

struct test_list {
    char *data;
};

TEST(list)
{
    struct test_list a, b;
    a.data = "123";
    b.data = "456";
    List_T *list = List_new(sizeof(struct test_list));
    ASSERT_EQ(List_push(list, &a), 1);
    ASSERT_EQ(List_push(list, &a), 2);
    ASSERT_EQ(List_push(list, &a), 3);
    List_free(list);
    list = List_new(sizeof(struct test_list));
    ASSERT_EQ(List_push(list, &a), 1);
    ASSERT_EQ(List_push(list, &a), 2);
    ASSERT_EQ(List_push(list, &a), 3);
    ASSERT_EQ(List_popleft(list, &b), 2);
    ASSERT_EQ(List_popleft(list, &b), 1);
    ASSERT_EQ(List_popleft(list, &b), 0);
    ASSERT_EQ(List_popleft(list, &b), -1);
    printf("%s\n", b.data);
    ASSERT_EQ(List_length(list), 0);
}

/* TEST(db) */
/* { */
/*     mail_t mail; */
/*     /\* strcpy(mail.subject, "nihao"); *\/ */
/*     /\* strcpy(mail.from, "123 <hongwozai@163.com>"); *\/ */
/*     /\* strcpy(mail.to, "\"我\" <hongwozai@163.com>"); *\/ */
/*     /\* mail.content = ""; *\/ */
/*     /\* mail.attach_name[0] = '\0'; *\/ */
/*     /\* mail.attach_content = ""; *\/ */
/*     db_init("192.168.100.34/LINUXORCL", "SCOTT", "123"); */
/*     /\* db_insert(&mail, "nihao", 0, 80); *\/ */
/*     int num, i; */
/*     char *word; */
/*     word = db_select(TYPE_KILL, KEY_FROM, &num); */
/*     for (i = 0; i < num; i++) { */
/*         printf("%s\n", word + 100 * i); */
/*     } */

/*     db_close(); */
/* } */

TEST(decode)
{
    char *p = "yPi077fWuavLvrXEt+e48cC51+i27tHHwrfU89HH";
    char str[1000];
    char ripestr[1000] = {0};
    char *rawp = str;
    char *ripep = ripestr;
    size_t rawlen;
    size_t ripelen;
    iconv_t trans;
    int len = base64_decode(p, strlen(p), str, 1000);

    trans = iconv_open("UTF-8", "GBK");
    if (!trans) {
        printf("shit, %s\n", strerror(errno));
        return;
    }
    str[len] = '\0';
    printf("str: %d, %s\n", len, str);

    /* transfer */
    rawlen = strlen(str);
    ripelen = 1000;
    printf("%d: %s, %d: %s\n", rawlen, rawp, ripelen, ripep);
    int flag = iconv(trans, &rawp, (size_t*)&rawlen,
                     &ripep, (size_t*)&ripelen);
    if (flag == -1) {
        printf("fuck, %s\n", strerror(errno));
        return;
    }
    ripestr[1000 - ripelen] = '\0';
    printf("ripe: %d, %s\n", flag, ripestr);
    iconv_close(trans);
}

struct test_shash {
    char *data;
};

TEST(shash)
{
    struct test_shash *find;
    struct test_shash test;
    Shash_T *a = Shash_new(10, sizeof(struct test_shash));
    test.data = "hello!";
    Shash_insert(a, "123",  &test);
    Shash_insert(a, "234",  &test);
    Shash_insert(a, "456",  &test);
    find = Shash_find(a, "123");
    ASSERT_NEQ(find, NULL);
    ASSERT_STR_EQ(find->data, "hello!");
    find = Shash_find(a, "456");
    ASSERT_NEQ(find, NULL);
    ASSERT_STR_EQ(find->data, "hello!");

    /* walk */
    char *key;
    struct test_shash *value;

    while (-1 != Shash_walk(a, &key, (void**)&value)) {
        printf("key: %s, value: %s\n", key, value->data);
    }
    Shash_free(a);
}

int amount = 0;
int ham_num = 0;
char buf[1024000];
void walk(const char *filename)
{
    int flag;
    long size;
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fuck!");
        return;
    }
    fread(buf, 102400, 1, fp);
    size = ftell(fp);
    buf[size] = '\0';
    ++amount;
    printf("size: %ld, filename: %s\n", size, filename);
    flag = spam_filter(buf);
    if (flag == 0) {
        ham_num++;
        printf("ham!\n");
    }
    fclose(fp);
}

TEST(spam_train)
{
    spam_init(0.8, "spam.data");
    spam_train("hw1_data/train");
    spam_close();
}

TEST(spam_filter)
{
    spam_init(0.8, "spam.data");
    walk_files("hw1_data/test/spam", walk);
    printf("amount: %d, ham_num: %d\n", amount, ham_num);
}

void unitest(void)
{
    TEST_ADD(spam_filter);
}
