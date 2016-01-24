/********************************************************************
 ** Copyright(c) 2015,哈尔滨工程大学信息安全研究中心
 ** All rights reserved
 **
 ** 文件名：db.c
 ** 创建人：王新
 ** 描  述：数据库部分
 ** 注  意：1.需要根据附件大小将mail中的那一项进行截断
 **
 ** 当前版本： v2.0
 ** 作    者：路泽亚
 ** 完成日期： 2015-12-22
 ** 修改记录：更改接口，修复bug
 **
 ** 当前版本： v1.0
 ** 作    者：王新
 ** 完成日期： 2015-12-21
 ********************************************************************/
#include <oci.h>
#include <stdio.h>
#include <iconv.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "inc/db.h"
#include "utils/mem.h"
#include "utils/systemlog.h"

#define SQL_MAXLINE  8000

static sword       status;
static OCIStmt    *mystmthp;
static OCISvcCtx  *mysvchp;
static OCIEnv     *myenvhp;
static OCIServer  *mysrvhp;
static OCIError   *myerrhp;
static OCISession *myusrhp;
static OCIParam   *mypardp;
static OCIDefine  *defkeywordp;

static void checkerr(sword status,OCIError *myerrhp);
static int  db_getn(int type, int keytype);
static void str_replace(char *p);
static void pretreatment(mail_t *mail, char *keyword);

int db_init(char *servername, char *username, char *passwd)
{
    OCIEnvCreate(&myenvhp, OCI_THREADED|OCI_OBJECT, (dvoid *)0, 0, 0, 0,
                 (size_t)0, (dvoid **)0);
    OCIHandleAlloc((dvoid *)myenvhp, (dvoid **)&mysrvhp, OCI_HTYPE_SERVER, 0,
                   (dvoid **)0);
    OCIHandleAlloc((dvoid *)myenvhp, (dvoid **)&myerrhp, OCI_HTYPE_ERROR, 0,
                   (dvoid **)0);
    /* 服务名 */
    status = OCIServerAttach(mysrvhp, myerrhp, (const OraText*)servername,
                             strlen(servername), OCI_DEFAULT);
        if (status != OCI_SUCCESS && status != OCI_SUCCESS_WITH_INFO){
        checkerr(status, myerrhp);
        OCIServerDetach(mysrvhp, myerrhp, OCI_DEFAULT);
        return -1;
    }
    OCIHandleAlloc((dvoid *)myenvhp, (dvoid **)&mysvchp, OCI_HTYPE_SVCCTX, 0,
                   (dvoid **)0);
    OCIAttrSet((dvoid *)mysvchp, OCI_HTYPE_SVCCTX, (dvoid *)mysrvhp, (ub4)0,
               OCI_ATTR_SERVER, myerrhp);
    OCIHandleAlloc((dvoid *)myenvhp, (dvoid **)&myusrhp, OCI_HTYPE_SESSION, 0,
                   (dvoid **)0);
    OCIAttrSet((dvoid *)myusrhp, OCI_HTYPE_SESSION, (dvoid *)username,
               (ub4)strlen(username), OCI_ATTR_USERNAME, myerrhp);
    OCIAttrSet((dvoid *)myusrhp, OCI_HTYPE_SESSION, (dvoid *)passwd,
               (ub4)strlen(passwd), OCI_ATTR_PASSWORD, myerrhp);

    if(OCISessionBegin((dvoid *)mysvchp, myerrhp, myusrhp, OCI_CRED_RDBMS,
                       OCI_DEFAULT) != OCI_SUCCESS) {
        DEBUG("Build the talking fail!\n");
        OCIHandleFree((void *)myenvhp, OCI_HTYPE_ENV);
        return -1;
    }
    OCIAttrSet((dvoid *)mysvchp, OCI_HTYPE_SVCCTX, (dvoid *)myusrhp, (ub4)0,
               OCI_ATTR_SESSION, myerrhp);
    INFO("Oracle connect successful!\n");
    return 0;
}

void  db_close()
{
    OCISessionEnd(mysvchp,myerrhp,myusrhp,OCI_DEFAULT);
    OCIServerDetach(mysrvhp,myerrhp,OCI_DEFAULT);
    OCIHandleFree((void *)myenvhp,OCI_HTYPE_ENV);
}

char* db_select(int type, int keytype, int *num)
{
    int i;
    text dkeyword[SQL_MAXCOLUMN];
    ub4 col_keyword_len=0;
    char *words;
    OraText sentence[SQL_MAXLINE];
    char table[20];
    char *sql =  "select keyword from %s where type=%d";

    *num = db_getn(type, keytype);
    if (*num == -1 || *num == 0) {
        return NULL;
    }
    strcpy(table, (type == TYPE_KILL )? "kill_key" : "monitor_key");
    sprintf((char*)sentence, sql, table, keytype);

    checkerr(OCIHandleAlloc((dvoid *)myenvhp, (dvoid **)&mystmthp,
                            OCI_HTYPE_STMT, 0, 0), myerrhp);
    checkerr(OCIStmtPrepare(mystmthp, myerrhp, sentence, strlen((char*)sentence),
                            OCI_NTV_SYNTAX, OCI_DEFAULT), myerrhp);

    checkerr(OCIStmtExecute(mysvchp, mystmthp, myerrhp, 0, 0, 0, 0,
                            OCI_DEFAULT), myerrhp);

    if (OCIParamGet(mystmthp, OCI_HTYPE_STMT, myerrhp, (void **)&mypardp,
                    1) != OCI_SUCCESS) {
        return NULL;
    }
    checkerr(OCIAttrGet((dvoid*)mypardp, OCI_DTYPE_PARAM,
                        (dvoid*)&col_keyword_len, 0, OCI_ATTR_DATA_SIZE,
                        myerrhp), myerrhp);

    if ((status = OCIDefineByPos(mystmthp, &defkeywordp, myerrhp, 1,
                               dkeyword, col_keyword_len + 1, SQLT_STR,
                               0, 0, 0, OCI_DEFAULT))) {
        checkerr(status, myerrhp);
        return NULL;
    }
    if (!CALLOC(words, *num, SQL_MAXCOLUMN)) {
        return NULL;
    }
    for(i = 0; i < *num; i++) {
        checkerr(OCIStmtFetch2(mystmthp, myerrhp, 1, OCI_FETCH_NEXT,
                               i, OCI_DEFAULT), myerrhp);
        strcpy(words + SQL_MAXCOLUMN*i, (char*)dkeyword);
    }
    return words;
}

int db_insert(mail_t *mail, char *keyword, int type, unsigned short port)
{
    OraText Insertp[SQL_MAXLINE];
    OCIBind *bind1 = NULL;
    OCIBind *bind2 = NULL;
    char table[20];
    char keywordselect[100];
    char *sql = "insert into %s values(id_seq.nextval,sysdate,'%s','%s','%s','%s', :1,'%s', :2,%d)";

    assert(mail);
    assert(keyword);
    if (type == TYPE_KILL) {
        strcpy(table, "kill_log");
    } else if (type == TYPE_MONITOR) {
        strcpy(table, "monitor_log");
    } else {
        strcpy(table, "spam_log");
    }
    strcpy(keywordselect, keyword);
    pretreatment(mail, keywordselect);
    /* 插入 */
    sprintf((char*)Insertp, sql, table, keywordselect, mail->from, mail->to,
            mail->subject, mail->attach_name, port);
    DEBUG("In Progressing:%s\n",Insertp);
    checkerr(OCIHandleAlloc((dvoid *)myenvhp, (dvoid **)&mystmthp,
                            OCI_HTYPE_STMT, (size_t)0, (dvoid **)0), myerrhp);
    checkerr(OCIStmtPrepare(mystmthp, myerrhp, Insertp,
                            (ub4)strlen((char*)Insertp),
                            (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT), myerrhp);
    DEBUG("content: %s\n attach_content: %s\n", mail->content,
          mail->attach_content);
    checkerr(OCIBindByPos(mystmthp, &bind1, myerrhp, 1,
                          mail->content, strlen(mail->content) + 1,
                          SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT),
             myerrhp);
    checkerr(OCIBindByPos(mystmthp, &bind2, myerrhp, 2,
                          mail->attach_content, strlen(mail->attach_content) + 1,
                          SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT),
             myerrhp);
    checkerr(OCIStmtExecute(mysvchp, mystmthp, myerrhp, (ub4)1, (ub4)0,
                            (CONST OCISnapshot *)0, (OCISnapshot *)0,
                            OCI_DEFAULT), myerrhp);
    checkerr(OCITransCommit(mysvchp, myerrhp, (ub4)0), myerrhp);

    assert(mail->content);
    assert(mail->attach_content);
    if (strcmp(mail->content, "") == 0) {
        mail->content = NULL;
    }
    if (strcmp(mail->attach_content, "") == 0) {
        mail->attach_content = NULL;
    }
    return 0;
}

static int db_getn(int type, int keytype)
{
    int        n = 0;
    char       table[20];
    char      *sql =  "select count(*) as n from %s where type=%d";
    OraText    row[4000];
    OCIDefine *dnp = NULL;

    strcpy(table, (type == TYPE_KILL)? "kill_key" : "monitor_key");
    sprintf((char*)row, sql,table, keytype);
    checkerr(OCIHandleAlloc((dvoid *)myenvhp, (dvoid **)&mystmthp,
                            OCI_HTYPE_STMT, (size_t)0, (dvoid **)0), myerrhp);
    checkerr(OCIStmtPrepare(mystmthp, myerrhp, row, (ub4)strlen((char*)row),
                            (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT), myerrhp);
    checkerr(OCIStmtExecute(mysvchp, mystmthp, myerrhp, (ub4)0, (ub4)0,
                            (CONST OCISnapshot *)0, (OCISnapshot *)0,
                            OCI_DEFAULT), myerrhp);
    if ((status = OCIDefineByPos(mystmthp, &dnp, myerrhp, 1, &n, sizeof(n),
                                 SQLT_INT, (dvoid *)0, 0, (ub2 *)0,
                                 OCI_DEFAULT))) {
        checkerr(status, myerrhp);
        return -1;
    }
    if(OCIStmtFetch2(mystmthp, myerrhp, 1, OCI_FETCH_NEXT, 0,
                     OCI_DEFAULT) != OCI_NO_DATA) {
        return n;
    }
    return 0;
}

static void checkerr(sword status,OCIError *myerrhp)
{
    text errbuf[512];
    sb4 errcode;

    switch(status)
    {
    case OCI_SUCCESS:
        break;
    case OCI_SUCCESS_WITH_INFO:
        OCIErrorGet((dvoid *)myerrhp, (ub4)1, (text *)NULL, &errcode,
                    errbuf, (ub4)sizeof(errbuf), (ub4)OCI_HTYPE_ERROR);
        ERROR("Error - OCI_SUCCESS_WITH_INFO\n%s\n", errbuf);
        break;
    case OCI_NEED_DATA:
        ERROR("Error - OCI_NEED_DATA\n");
        break;
    case OCI_NO_DATA:
        ERROR("Error - OCI_NO_DATA\n");
        break;
    case OCI_ERROR:
        OCIErrorGet((dvoid *)myerrhp, (ub4)1, (text *)NULL, &errcode,
                    errbuf, (ub4)sizeof(errbuf), (ub4)OCI_HTYPE_ERROR);
        ERROR("Error - OCI_ERROR\n%s\n",errbuf);
        break;
    case OCI_INVALID_HANDLE:
        ERROR("Error - OCI_INVALID_HANDLE\n");
        break;
    case OCI_STILL_EXECUTING:
        ERROR("Error - OCI_STILL_EXECUTING\n");
        break;
    case OCI_CONTINUE:
        ERROR("Error - OCI_CONTINUE\n");
        break;
    default:
        break;
    }
}

static void str_replace(char *p)
{
    for (; *p != '\0'; p++)
        if (*p == '\'' || *p == '\"' || *p == '\n' ||
            *p == '<' || *p == '>')
            *p = ' ';
}

static char* trans(char *ptr, int type)
{
    int flag;
    char *start = ptr;
    size_t rawlen, ripelen;
    char *rawcode;
    char decode[3008];
    char *decodep = decode;
    iconv_t trans;

    if (!ptr)
        return NULL;
    if (*ptr == '\0')
        return NULL;
    trans = iconv_open("UTF-8//IGNORE", "GBK");
    if (!trans) {
        ERROR_NO("iconv open error!");
        return NULL;
    }
    rawcode = (char*)ptr;
    rawlen = strlen(ptr);
    rawlen = (rawlen > 2000) ? 2000 : rawlen;
    ripelen = 3000;
    DEBUG("rawlen: %d, rawcode: %s\n", rawlen, rawcode);
    flag = iconv(trans, &rawcode, (size_t*)&rawlen,
                 &decodep, (size_t*)&ripelen);
    if (flag == -1) {
        DEBUG("iconv transfer error!(%s)\n", strerror(errno));
        if (type == 0) {
            start[rawlen] = '\0';
            DEBUG("start: %p\n", start);
            return start;
        }
        return NULL;
    }
    decode[2999 - ripelen + 1] = '\0';
    DEBUG("flag: %d, decodep: %s\n", ripelen, decode);
    if (type == 0) {
        REALLOC(ptr, ptr, 2999 - ripelen + 2);
        strcpy(ptr, decode);
        iconv_close(trans);
        return ptr;
    }
    strcpy(ptr, decode);
    iconv_close(trans);
    return NULL;
}

static void pretreatment(mail_t *mail, char *keyword)
{
    /* 处理为空的情况 */
    /* 两个utf8字时会出现可以转换的问题 */
    /* trans(keyword, 1); */
    /* trans(mail->from, 1); */
    /* trans(mail->to, 1); */
    /* trans(mail->subject, 1); */
    /* mail->content = trans(mail->content, 0); */
    /* trans(mail->attach_name, 1); */
    /* mail->attach_content = trans(mail->attach_content, 0); */
    DEBUG("mail->content: %s\n", mail->content);
    if (mail->content == NULL) {
        mail->content = "";
    }
    if (mail->attach_content == NULL) {
        mail->attach_content = "";
    }
    if (strlen(mail->from) > 100) {
        mail->from[99] = '\0';
    }
    if (strlen(mail->to) > 100) {
        mail->to[99] = '\0';
    }
    if (strlen(mail->subject) > 100) {
        mail->subject[99] = '\0';
    }
    if (strlen(mail->content) > 3000) {
        mail->content[2999] = '\0';
    }
    if (strlen(mail->attach_content) > 3000) {
        mail->attach_content[2999] = '\0';
    }
    /* 处理'" */
    str_replace(mail->from);
    str_replace(mail->to);
    str_replace(mail->subject);
    str_replace(mail->content);
    str_replace(mail->attach_name);
    str_replace(mail->attach_content);
}
