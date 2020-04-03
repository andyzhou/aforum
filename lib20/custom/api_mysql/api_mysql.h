/*
* Copyright (c) 2006
* All rights reserved.
* 
* 文件名称：api_mysql.h
* 文件标识：数据库操作接口
* 作    者：周志锐(丢丢)
*/

#ifndef _API_MYSQL_H
#define _API_MYSQL_H

#include "mysql.h"

///////////////////////////

//单个数据库链接参数结构
typedef struct {
	MYSQL *pConn;//数据库连接句柄
	MYSQL_RES *pResult;//数据集句柄
	MYSQL_ROW pRow;//单行数据集
	int  db_error_num;
}mysql_handle;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct db_cfg {
	char db_server[32];
	char db_user[32];
	char db_pwd[32];
	char db_name[32];
	char db_sock[32];
	int  db_port;
	int  db_no;
	int  rand_service_no;
}db_cfg;

//数据库配置
db_cfg cfg;
db_cfg dbCfg;

/////
//扩展查询参数结构
//检查某个字段的值是否符合匹配的条件
//仅针对使用 组合信息形成的字段
//查询、匹配前缀值, 若不匹配，则不会继续查询、保存数据
typedef struct sql_ext {
	int check_rec;//1:check 0:not check
	int col_num;//column no, from 1 start
	int fix_val;//will be fixed int val
}sql_ext;


/////////////////////////

///记录链表
struct listNode{ 
   table* col_list;
   struct listNode *nextPtr; 
}; 

typedef struct listNode LISTNODE; 
typedef LISTNODE * DbRecordList; 


////////MYSQL原始接口//////////////////////////////////////


//数据库链接
//返回值: 1成功 0失败
int mysqlConnect(db_cfg cfg, mysql_handle* mh);
int mysqlSimpConnect(db_cfg cfg, mysql_handle* mh);

//SQL语句执行
//返回值: 1成功 0失败
int mysqlQuery(pool *p, mysql_handle* mh, const char *sql, char **errBuf);
int mysqlSimpQuery(pool *p, mysql_handle* mh, const char *sql, char **errBuf);

//存储记录集模块
int mysqlStoreResult(mysql_handle* mh);

//获得结果数模块
int mysqlNumRows(mysql_handle* mh);

//记录变量模块
MYSQL_ROW mysqlFetchRow(mysql_handle* mh);

//符合的字段数
int mysqlFetchCol(mysql_handle* mh);

//释放数据库结果集模块 
void mysqlFreeResult(mysql_handle* mh);

//关闭mysql
void mysqlClose(mysql_handle* mh);

////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif	/* !_API_MYSQL_H */
