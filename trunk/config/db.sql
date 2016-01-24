-- type 代表 关键字类型 0-5（发件人，收件人，主题，内容，附件名，附件内容）
-- verify 0 代表未审核， 1 代表审核过
create table T_USER(
        username VARCHAR2(100) primary key,
        password VARCHAR2(100),
        priority NUMBER,
        verify NUMBER
        );

create table MONITOR_KEY(
        monitor_id NUMBER,
        type NUMBER,
        keyword VARCHAR2(100),
        primary key (type, keyword)
        );

create table KILL_KEY(
        kill_id NUMBER,
        type NUMBER,
        keyword VARCHAR2(100),
        primary key (type, keyword)
        );

create table MONITOR_LOG(
        id NUMBER primary key,
        datatime DATE,
        keyword VARCHAR2(100),
        sender VARCHAR2(100),
        receiver VARCHAR2(100),
        subject VARCHAR2(100),
        content VARCHAR2(3000),
        attachmentsubject VARCHAR2(100),
        attachmentcontent VARCHAR2(3000),
        protocol NUMBER);

create table KILL_LOG(
        id NUMBER primary key,
        datatime DATE,
        keyword VARCHAR2(100) ,
        sender VARCHAR2(100),
        receiver VARCHAR2(100),
        subject VARCHAR2(100),
        content VARCHAR2(3000),
        attachmentsubject VARCHAR2(100),
        attachmentcontent VARCHAR2(3000),
        protocol NUMBER);

create table SPAM_LOG(
        id NUMBER primary key,
        datatime DATE,
        keyword VARCHAR2(100),
        sender VARCHAR2(100),
        receiver VARCHAR2(100),
        subject VARCHAR2(100),
        content VARCHAR2(3000),
        attachmentsubject VARCHAR2(100),
        attachmentcontent VARCHAR2(3000),
        protocol NUMBER);

create sequence id_seq;

create sequence kill_id_seq;

create sequence monitor_id_seq;


-- 插入语句示例
-- sqlplus显示格式
column ID format a10;
column DATATIME format a10;
column SENDER format a10;
column ID format a10;
column ID format a10;
column ID format a10;
