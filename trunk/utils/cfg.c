/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：cfg.c
 ** 创建人：路泽亚
 ** 描  述：配置文件的解析很简单,就是节点下面两个链表
 ** 注  意：本程序还待重写
 **
 ** 当前版本： v1.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-11-27
 **
 ** 版本： v0.9
 ** 作    者：路泽亚
 ** 完成日期： 2015-07-23
 ********************************************************************/
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "systemlog.h"

/**
 * 为了同之前写的代码兼容
 */
#define err(str, args...) \
    ERROR_NO(str, ##args)

/* ======================= 各种类型 ========================= */
#define LINE_MAX_CHAR (1024)
#define KV_MAX_VALUE (1024)

typedef enum {
    LS_TYPE = 1,                /* 左方括号 */
    RS_TYPE,                    /* 右方括号 */
    COMMENT_TYPE,               /* 注释符号 */
    STRING_TYPE,                /* 字符串 */
    NEWLINE_TYPE,               /* 换行 */
    ASSIGN_TYPE,                 /* 赋值符号= */
    END_TYPE
} TokenType;

typedef enum {
    INITIAL_STATE = 1,          /* 初始状态 */
    COMMENT_STATE,              /* 注释状态 */
    STRING_STATE,               /* 字符串状态 */
} LexState;

typedef struct {
    TokenType type;
    char *string;
} Token;

typedef struct {
    char *key;
    char *value;
} Pair;

typedef struct PairList_tag {
    Pair pair;
    struct PairList_tag *next;
} PairList;

typedef struct {
    char *section_name;
    PairList *pair_list;
} Section;

typedef struct SectionList_tag {
    Section section;
    struct SectionList_tag *next;
} SectionList;

typedef struct {
    Token token;               /* 当前token */
    int buf_exists;
} Parse;

struct Cfg_tag {
    int line_no;
    FILE *fp;
    /* =======lex parse =========*/
    SectionList *section_list;
};

/* ====================== 词法分析 ======================== */
static void
token_out_space(char *str)
{
    int len = strlen(str);
    for (str += len-1; *str == ' '; str--);
    str++;
    *str = '\0';
}

static void
scan(struct Cfg_tag *cfg, Token *token)
{
    char ch;
    int  pos = 0;
    LexState state = INITIAL_STATE;

    assert(NULL != cfg);
    assert(NULL != token);

    for (ch = getc(cfg->fp); !feof(cfg->fp); ch = getc(cfg->fp)) {
        if (state == INITIAL_STATE) {
            if (ch == '#') {
                state = COMMENT_STATE;
            } else if (isalnum(ch)) {
                state = STRING_STATE;
                token->type  =  STRING_TYPE;
                token->string = (char*)malloc(1024);
                if (!token->string) {
                    err("token string malloc error!");
                    exit(1);
                }
                token->string[pos++] = ch;
            } else if (isspace(ch)) {
                if (ch == '\n') {
                    cfg->line_no++;
                    token->type = NEWLINE_TYPE;
                    return;
                }
            } else if (ch == '['){
                token->type = LS_TYPE;
                return;
            } else if (ch == ']') {
                token->type = RS_TYPE;
                return;
            } else if (ch == '='){
                token->type = ASSIGN_TYPE;
                return;
            } else {
                err("line %d:INITIAL STATE error!", cfg->line_no);
                exit(1);
            }
        } else if (state == COMMENT_STATE) {
            if (ch != '\n') {
                continue;
            }
            /* comment状态下的换行符忽略，但行数不忽略 */
            state = INITIAL_STATE;
            cfg->line_no++;
        } else if (state == STRING_STATE) {
            if (ch != ']' && ch != '\n' && ch != '=') {
                token->string[pos++] = ch;
                continue;
            }
            token->string[pos] = '\0';
            ungetc(ch, cfg->fp);
            token_out_space(token->string);
            return;
        } else {
            err("line %d: scan error! error! error!", cfg->line_no);
            exit(1);
        }
    }
    /* 当string后跟结束符时，会跳出循环，所以要修正 */
    if (state == INITIAL_STATE) {
        token->type = END_TYPE;
    }
}

static void
get_token(struct Cfg_tag *cfg, Parse *parse, Token *token)
{
    if (parse->buf_exists) {
        *token = parse->token;
        parse->buf_exists = 0;
    } else {
        scan(cfg, token);
    }
}
static void
unget_token(Parse *parse, Token *token)
{
    parse->buf_exists = 1;
    parse->token = *token;
}


/* ====================== 语法分析 ============================ */
static void
parse_pair(struct Cfg_tag *cfg, Parse *parse, Pair *pair)
{
    Token token;

    /* 确定为此产生式 */
    get_token(cfg, parse, &token);
    pair->key = token.string;
    get_token(cfg, parse, &token);
    if (token.type != ASSIGN_TYPE) {
        err("line %d: forget '=' !", cfg->line_no);
        exit(1);
    }
    get_token(cfg, parse, &token);
    if (token.type != STRING_TYPE) {
        err("line %d: key = value !", cfg->line_no);
        exit(1);
    }
    pair->value = token.string;
    get_token(cfg, parse, &token);
    if (token.type != NEWLINE_TYPE && token.type != END_TYPE) {
        err("line %d: must be have a '\n' !", cfg->line_no);
        exit(1);
    }
}

/* 未对pair_list分配内存 */
static void
parse_pair_list(struct Cfg_tag *cfg, Parse *parse, PairList *pair_list)
{
    Token token;
    PairList *prev = NULL;        /* 用来处理结尾处的next */

    /* 第一次读的一定正确， 这个由上层保证 */
    for (get_token(cfg, parse, &token);
         token.type == STRING_TYPE || token.type == NEWLINE_TYPE;
         get_token(cfg, parse, &token)) {
        if (token.type == NEWLINE_TYPE)
            continue;
        unget_token(parse, &token);
        parse_pair(cfg, parse, &pair_list->pair);
        pair_list->next = (PairList*)malloc(sizeof(PairList));
        if (!pair_list->next) {
            err("malloc error!");
            exit(1);
        }
        prev = pair_list;
        pair_list = pair_list->next;
    }
    unget_token(parse, &token);
    free(prev->next);
    prev->next = NULL;
}

/* 未给section_name分配内存, 需要给token的string分配内存 */
static void
parse_section_name(struct Cfg_tag *cfg, Parse *parse, Section *section)
{
    Token token;

    get_token(cfg, parse, &token); /* 是[ */
    get_token(cfg, parse, &token);
    if (token.type != STRING_TYPE) {
        err("line %d: section name error!", cfg->line_no);
        exit(1);
    }
    section->section_name = token.string;
    get_token(cfg, parse, &token);
    if (token.type != RS_TYPE) {
        err("line %d: section ] forget!", cfg->line_no);
        exit(1);
    }
    get_token(cfg, parse, &token);
    if (token.type != NEWLINE_TYPE) {
        err("line %d: section name must be a line!", cfg->line_no);
        exit(1);
    }
}

static void
parse_section(struct Cfg_tag *cfg, Parse *parse, Section *section)
{
    Token token;

    parse_section_name(cfg, parse, section);
    for (get_token(cfg, parse, &token);
         token.type == NEWLINE_TYPE;
         get_token(cfg, parse, &token)){
        continue;
    }
    if (token.type != STRING_TYPE) {
        err("line %d: not have null section.", cfg->line_no);
        exit(1);
    }
    unget_token(parse, &token);
    section->pair_list = (PairList*)malloc(sizeof(PairList));
    if (!section->pair_list) {
        err("parse_section malloc error!");
        exit(1);
    }
    unget_token(parse, &token);
    parse_pair_list(cfg, parse, section->pair_list);
}

static void
parse_section_list(struct Cfg_tag *cfg, Parse *parse, SectionList *section_list)
{
    Token token;
    SectionList *prev = NULL;

    for (get_token(cfg, parse, &token);
         token.type == LS_TYPE || token.type == NEWLINE_TYPE;
         get_token(cfg, parse, &token)) {
        if (token.type == NEWLINE_TYPE)
            continue;
        unget_token(parse, &token);
        parse_section(cfg, parse, &section_list->section);
        section_list->next = (SectionList*)malloc(sizeof(SectionList));
        if (!section_list->next) {
            err("parse_section_list malloc error!\n");
            exit(1);
        }
        prev = section_list;
        section_list = section_list->next;
    }
    unget_token(parse, &token);
    free(prev->next);
    prev->next = NULL;
}

static void
parse_cfg(struct Cfg_tag *cfg, Parse *parse)
{
    Token token;

    cfg->section_list  = (SectionList*)malloc(sizeof(SectionList));
    if (!cfg->section_list) {
        err("parse malloc error!");
        exit(1);
    }
    get_token(cfg, parse, &token);
    if (token.type != LS_TYPE) {
        err("line %d: cfg must be have a section!", cfg->line_no);
        exit(1);
    }
    unget_token(parse, &token);
    parse_section_list(cfg, parse, cfg->section_list);
    get_token(cfg, parse, &token);
    if (token.type != END_TYPE) {
        err("line before %d: cfg error! error! error!", cfg->line_no);
        exit(1);
    }
}

/* ================== 内存释放部分 =================== */
/* TODO: 由于一开始没有考虑内存问题，导致后期进行遍历释放内存，十分麻烦 */
static void
cfg_free_pair(Pair *p)
{
    assert(NULL != p);
    free(p->key);
    free(p->value);
}

static void
cfg_free_pair_list(PairList *pl)
{
    PairList *p;

    assert(NULL != pl);
    do
    {
        p = pl;
        pl = pl->next;
        cfg_free_pair(&p->pair);
        free(p);
    } while (pl);
}

static void
cfg_free_section(Section *s)
{
    assert(NULL != s);
    free(s->section_name);
    cfg_free_pair_list(s->pair_list);
}

static void
cfg_free_section_list(SectionList *sl)
{
    SectionList *p;

    assert(NULL != sl);
    do {
        p = sl;
        sl = sl->next;
        cfg_free_section(&p->section);
        free(p);
    } while (sl);
}

/* ======================== 具体的逻辑实现部分 ======================= */
void
cfg_open(struct Cfg_tag **cfg, const char *file)
{
    Parse parse;

    *cfg = (struct Cfg_tag*)malloc(sizeof(struct Cfg_tag));
    if (!*cfg) {
        err("cfg memory alloc error!\n");
        exit(1);
    }
    parse.buf_exists = 0;
    (*cfg)->line_no = 1;
    (*cfg)->fp = fopen(file, "r");
    if (!(*cfg)->fp) {
        err("%s: file can't open.", file);
        exit(1);
    }
    parse_cfg(*cfg, &parse);
}


void
cfg_close(struct Cfg_tag *cfg)
{
    if (cfg) {
        fclose(cfg->fp);
        cfg_free_section_list(cfg->section_list);
        free(cfg);
    }
}

/* 直接遍历链表 */
int
cfg_read(struct Cfg_tag *cfg, const char *section, const char *key, char *value)
{
    SectionList *s;
    PairList    *p;

    for (s = cfg->section_list; s; s = s->next)
        if (strcmp(s->section.section_name, section) == 0)
            for (p = s->section.pair_list; p; p = p->next)
                if (strcmp(p->pair.key, key) == 0) {
                    strcpy(value, p->pair.value);
                    return 0;
                }
    return -1;
}

int
cfg_readint(const char *file, const char *section, const char *key,
            int *value)
{
    struct Cfg_tag *cfg;
    int flag;
    char buf[43];

    cfg_open(&cfg, file);
    flag = cfg_read(cfg, section, key, buf);
    cfg_close(cfg);
    *value = atoi(buf);
    return flag;
}

int
cfg_readstring(const char *file, const char *section, const char *key,
               char *value)
{
    struct Cfg_tag *cfg;
    int flag;

    cfg_open(&cfg, file);
    flag = cfg_read(cfg, section, key, value);
    cfg_close(cfg);
    return flag;
}
