/*
* Copyright (c) 2006
* All rights reserved.
* 
* 文件名称：api_mysql.c
* 文件标识：数据库操作接口
* 作    者：周志锐(丢丢)
*/

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_main.h"
#include "http_log.h"
#include "http_protocol.h"
#include "ap_alloc.h"

#include "mysql.h"
#include "api_mysql.h"

MYSQL *mydb;//数据库内部实例化对象
MYSQL_RES *res;//数据记录集指针
MYSQL_ROW myrow;//数据记录行


//创建数据库连接句柄实例, 保持连接状态
#define MYSQL_SERVICE_NUM 3

mysql_handle zic_global_mhandler[MYSQL_SERVICE_NUM];
mysql_handle zic_u_mhandler[MYSQL_SERVICE_NUM];
mysql_handle zic_g_mhandler[MYSQL_SERVICE_NUM];

//extern db_cfg cfg;

//////////////////////////////////////////

//私有函数///////

//判断sql语句类型
//返回值：1: select 2: insert 3: update 4:delete
//		  -1: 缺少sql语句 0:无效类型
int sql_str_type(const char *sql_str);


///////////////////////////////////////////

//获得mysql链接hash句柄
//获得某个值之间的随机值
//返回随机数
int get_rand_num(int end_num)
{
	
	int rand_num = 0;
	time_t timeNow;
	
	//获得随机时间
	srand(time(&timeNow));
	
	//产生随机数
	rand_num = rand() % end_num;

	return rand_num;
}


//判断sql语句类型
//返回值：1: select 2: insert 3: update 4:delete
//		  -1: 缺少sql语句 0:无效类型
int sql_str_type(const char *sql_str)
{
	char *p;
	char sql_type_str[10] = {0};
	
	if(!sql_str || strlen(sql_str) < 10)
	{
		return -1;
	}
	
	p = strchr(sql_str, ' ');
	
	if((p - sql_str) != 6)
	{
		return 0;
	}
	
	memset(sql_type_str, 0, 10);
	strncpy(sql_type_str, sql_str, p-sql_str);
	
	//匹配sql类型
	if(strcmp(sql_type_str, "select") == 0)
	{
		return 1;
	}
	
	if(strcmp(sql_type_str, "insert") == 0)
	{
		return 2;
	}
	
	if(strcmp(sql_type_str, "update") == 0)
	{
		return 3;
	}
	
	if(strcmp(sql_type_str, "delete") == 0)
	{
		return 4;
	}
	
	return 0;
}

//数据库链接
//返回值: 1成功 0失败
/*
int mysqlConnect(pool *p, char *dbServer, char *dbUser, char *dbPwd, 
				 char *dbName, int dbPort, char **errBuf)
*/
int mysqlConnect(db_cfg cfg, mysql_handle *mh)
{
	//参数检验

	if(!cfg.db_user || !strcmp(cfg.db_user, ""))
	{
		return 0;
	}

	if(!cfg.db_pwd || !strcmp(cfg.db_pwd, ""))
	{
		return 0;
	}

	if(!cfg.db_server || !strcmp(cfg.db_server, ""))
	{
		sprintf(cfg.db_server, "localhost");
	}

	if(cfg.db_port <= 0)
	{
		cfg.db_port = 3306;
	}
	
	/*
	//打开MYSQL连接函数		
	if((mydb = mysql_init((MYSQL*)0)) && mysql_real_connect(mydb, dbServer, dbUser, dbPwd, dbName, dbPort, NULL, 0))
	{
		//连接数据库成功
		//*errBuf = (char*)ap_palloc(p, 256);
		//*errBuf = (char*)ap_pcalloc(p, 256);
		
		//*errBuf = ap_psprintf(p, "%s", (char *)mysql_error(mydb));
		return 1;
	}
	*/

#if MYSQL_VERSION_ID >= 32200

	mh->pConn = mysql_init(NULL);
	
	if(!mh->pConn)
	{
		return 0;
	}

	if(mysql_real_connect(mh->pConn, cfg.db_server, cfg.db_user,	
		cfg.db_pwd, cfg.db_name, cfg.db_port, NULL, 0))
	{
		//连接成功
		return 1;
	}

#else

	if(mysql_real_connect(mh->pConn, cfg.db_server, cfg.db_user,
		cfg.db_pwd, cfg.db_port, NULL, 0))
	{
		return 1;
	}
#endif
	
	/*
	*errBuf = (char*)ap_palloc(p, 256);
	*errBuf = (char*)ap_pcalloc(p, 256);		
	*errBuf = ap_psprintf(p, "%s", (char *)mysql_error(mydb));
	*/
	
	return 0;
}


mysqlSimpConnect(db_cfg cfg, mysql_handle* mh)
{
	
	//参数检验
	if(!cfg.db_user || !strcmp(cfg.db_user, ""))
	{
		return 0;
	}

	if(!cfg.db_pwd || !strcmp(cfg.db_pwd, ""))
	{
		return 0;
	}

	if(!cfg.db_server || !strcmp(cfg.db_server, ""))
	{
		sprintf(cfg.db_server, "localhost");
	}

	if(cfg.db_port <= 0)
	{
		cfg.db_port = 3306;
	}

	/*
	//打开MYSQL连接函数		
	if((mydb = mysql_init((MYSQL*)0)) && mysql_real_connect(mydb, dbServer, dbUser, dbPwd, dbName, dbPort, NULL, 0))
	{
		//连接数据库成功
		//*errBuf = (char*)ap_palloc(p, 256);
		//*errBuf = (char*)ap_pcalloc(p, 256);
		
		//*errBuf = ap_psprintf(p, "%s", (char *)mysql_error(mydb));
		return 1;
	}
	*/

#if MYSQL_VERSION_ID >= 32200

	mh->pConn = mysql_init(NULL);
	
	if(!mh->pConn)
	{
		return 0;
	}

	if(mysql_real_connect(mh->pConn, cfg.db_server, cfg.db_user,	
		cfg.db_pwd, cfg.db_name, cfg.db_port, cfg.db_sock, 0))
	{
		//连接成功
		return 1;
	}

#else

	if(mysql_real_connect(mh->pConn, cfg.db_server, cfg.db_user,
		cfg.db_pwd, cfg.db_port, NULL, 0))
	{
		return 1;
	}
#endif

	//ap_rprintf(r, "<h1>error:%s</h1>", (char *)mysql_error(mh->pConn));
	
	/*
	*errBuf = (char*)ap_palloc(p, 256);
	*errBuf = (char*)ap_pcalloc(p, 256);		
	*errBuf = ap_psprintf(p, "%s", (char *)mysql_error(mydb));
	*/
	
	return 0;
}


//SQL语句执行
//返回值: 1成功 0失败
int mysqlQuery(pool *p, mysql_handle *mh, const char *sql, char **errBuf)
{	
	return 1;
	
	if(!mh || !sql)
	{
		return 0;
	}

	mh->pResult = NULL;


	//执行成功 
	if(!mysql_query(mh->pConn, sql)) 
	{ 
		if(mysql_errno(mh->pConn) == 0)
		{
			return 1;
		}
	}
	
	*errBuf = (char*)ap_palloc(p, 256);
	*errBuf = (char*)ap_pcalloc(p, 256);
	
	*errBuf = ap_psprintf(p, "error:%s", (char *)mysql_error(mh->pConn));

	return 0; 
}



int mysqlSimpQuery(pool *p, mysql_handle *mh, const char *sql, char **errBuf)
{
	
	if(!mh->pConn || !sql || !(*sql))
	{
		return 0;
	}

	/*
	if(mh->pResult)
	{
		mh->pResult = NULL;
	}
	*/

	//执行成功 
	if(!mysql_query(mh->pConn, sql)) 
	{ 
		if(mysql_errno(mh->pConn) == 0)
		{
			return 1;
		}
	}
	
	*errBuf = (char*)ap_palloc(p, 256);
	*errBuf = (char*)ap_pcalloc(p, 256);
	
	*errBuf = ap_psprintf(p, "%s", (char *)mysql_error(mh->pConn));

	return 0; 
}

//存储记录集模块
int mysqlStoreResult(mysql_handle *mh)
{
	if(!mh->pConn)
	{
		return 0;
	}

	//进行结果存储 
	mh->pResult=mysql_store_result(mh->pConn); 
	
	if(mh->pResult) 
	{ 
		return 1; 
	} 
	
	return 0;
}

//获得结果数模块
int mysqlNumRows(mysql_handle *mh)
{
	int num;//记录数 
	
	//调用结果存储 
	if(mysqlStoreResult(mh)) 
	{ 
		num = mysql_num_rows(mh->pResult); 
		
		return num;//返回符合要求的记录数 
	} 
	
	return 0;//返回空记录数 
}

//符合的字段数
int mysqlFetchCol(mysql_handle *mh)
{
	if(!mh->pResult)
	{
		return 0;
	}
	
	return mysql_num_fields(mh->pResult);
}

//记录变量模块
MYSQL_ROW mysqlFetchRow(mysql_handle *mh)
{
	if(!mh->pResult) 
	{ 
		return NULL; 
	} 
	
	return mysql_fetch_row(mh->pResult);
}

//释放数据库结果集模块 
void mysqlFreeResult(mysql_handle *mh)
{
	if(mh->pResult) 
	{ 
		mysql_free_result(mh->pResult);
	}
}

//关闭mysql
void mysqlClose(mysql_handle *mh)
{
	if(mh->pConn)
	{
		mysql_close(mh->pConn);
		mh->pConn = NULL;
	}
}
