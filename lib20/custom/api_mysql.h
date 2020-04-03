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

#ifdef WIN32
#include "mysql/mysql.h"
#else
#include "mysql.h"
#endif

///////////////////////////

//单个数据库链接参数结构
typedef struct {
	MYSQL *pConn;//数据库连接句柄
	MYSQL_RES *pResult;//数据集句柄
	MYSQL_ROW pRow;//单行数据集
	int  isConnect;//是否已经链接成功
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
   apr_table_t* col_list;
   struct listNode *nextPtr; 
}; 

typedef struct listNode LISTNODE; 
typedef LISTNODE * DbRecordList; 


////////MYSQL原始接口//////////////////////////////////////


//数据库链接
//返回值: 1成功 0失败
int mysqlConnect(db_cfg cfg, mysql_handle* mh);
int mysqlSimpConnect(apr_pool_t *p, db_cfg cfg, mysql_handle* mh, char **errBuf);

//SQL语句执行
//返回值: 1成功 0失败
int mysqlQuery(apr_pool_t *p, mysql_handle* mh, const char *sql, char **errBuf);
int mysqlSimpQuery(apr_pool_t *p, mysql_handle* mh, const char *sql, char **errBuf);

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


//初始化、保存单个记录内存空间
DbRecordList recordList_new(request_rec* r, DbRecordList sPtr, apr_table_t* rec_list);

/////////////////////////

//数据库初始化
//成功1 失败返回0
int dbInit(apr_pool_t* p, db_cfg cfg, char** err_buf);

//仅执行插入、更新、删除SQL命令
//成功1 失败返回0
int dbQuery(apr_pool_t* p, db_cfg cfg, const char* sql, char** err_buf);

int dbQueryTest(request_rec* r, db_cfg cfg, const char* sql, char** err_buf);


//获得结果集
//成功返回匹配结果数 失败返回0
int dbGetRecords(request_rec* r, db_cfg cfg, const char* sql,
				 DbRecordList* sPtr, char** err_buf);


//扩展查询，获得结果集
int dbGetRecordsExtend(request_rec* r, db_cfg cfg, const char* sql,
					   sql_ext sqex, DbRecordList* sPtr, char** err_buf);

////////////////////////////////

//060409 查询key=value格式的数据到table
//成功返回匹配结果数 失败返回0
int dbGetHashRecords(request_rec* r, db_cfg cfg, const char* sql,
					 		apr_table_t** suitHashRec, char** err_buf);


//分割单条记录各个字段到table
int recordSplit(request_rec* r, char* signal_record, apr_table_t** col_list);

#ifdef __cplusplus
}
#endif

#endif	/* !_API_MYSQL_H */
