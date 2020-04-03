/*
* Copyright (c) 2006
* All rights reserved.
* 
* �ļ����ƣ�api_mysql.c
* �ļ���ʶ�����ݿ�����ӿ�
* ��    �ߣ���־��(����)
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

MYSQL *mydb;//���ݿ��ڲ�ʵ��������
MYSQL_RES *res;//���ݼ�¼��ָ��
MYSQL_ROW myrow;//���ݼ�¼��


//�������ݿ����Ӿ��ʵ��, ��������״̬
#define MYSQL_SERVICE_NUM 3

mysql_handle zic_global_mhandler[MYSQL_SERVICE_NUM];
mysql_handle zic_u_mhandler[MYSQL_SERVICE_NUM];
mysql_handle zic_g_mhandler[MYSQL_SERVICE_NUM];

//extern db_cfg cfg;

//////////////////////////////////////////

//˽�к���///////

//�ж�sql�������
//����ֵ��1: select 2: insert 3: update 4:delete
//		  -1: ȱ��sql��� 0:��Ч����
int sql_str_type(const char *sql_str);


///////////////////////////////////////////

//���mysql����hash���
//���ĳ��ֵ֮������ֵ
//���������
int get_rand_num(int end_num)
{
	
	int rand_num = 0;
	time_t timeNow;
	
	//������ʱ��
	srand(time(&timeNow));
	
	//���������
	rand_num = rand() % end_num;

	return rand_num;
}


//�ж�sql�������
//����ֵ��1: select 2: insert 3: update 4:delete
//		  -1: ȱ��sql��� 0:��Ч����
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
	
	//ƥ��sql����
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

//���ݿ�����
//����ֵ: 1�ɹ� 0ʧ��
/*
int mysqlConnect(pool *p, char *dbServer, char *dbUser, char *dbPwd, 
				 char *dbName, int dbPort, char **errBuf)
*/
int mysqlConnect(db_cfg cfg, mysql_handle *mh)
{
	//��������

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
	//��MYSQL���Ӻ���		
	if((mydb = mysql_init((MYSQL*)0)) && mysql_real_connect(mydb, dbServer, dbUser, dbPwd, dbName, dbPort, NULL, 0))
	{
		//�������ݿ�ɹ�
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
		//���ӳɹ�
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
	
	//��������
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
	//��MYSQL���Ӻ���		
	if((mydb = mysql_init((MYSQL*)0)) && mysql_real_connect(mydb, dbServer, dbUser, dbPwd, dbName, dbPort, NULL, 0))
	{
		//�������ݿ�ɹ�
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
		//���ӳɹ�
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


//SQL���ִ��
//����ֵ: 1�ɹ� 0ʧ��
int mysqlQuery(pool *p, mysql_handle *mh, const char *sql, char **errBuf)
{	
	return 1;
	
	if(!mh || !sql)
	{
		return 0;
	}

	mh->pResult = NULL;


	//ִ�гɹ� 
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

	//ִ�гɹ� 
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

//�洢��¼��ģ��
int mysqlStoreResult(mysql_handle *mh)
{
	if(!mh->pConn)
	{
		return 0;
	}

	//���н���洢 
	mh->pResult=mysql_store_result(mh->pConn); 
	
	if(mh->pResult) 
	{ 
		return 1; 
	} 
	
	return 0;
}

//��ý����ģ��
int mysqlNumRows(mysql_handle *mh)
{
	int num;//��¼�� 
	
	//���ý���洢 
	if(mysqlStoreResult(mh)) 
	{ 
		num = mysql_num_rows(mh->pResult); 
		
		return num;//���ط���Ҫ��ļ�¼�� 
	} 
	
	return 0;//���ؿռ�¼�� 
}

//���ϵ��ֶ���
int mysqlFetchCol(mysql_handle *mh)
{
	if(!mh->pResult)
	{
		return 0;
	}
	
	return mysql_num_fields(mh->pResult);
}

//��¼����ģ��
MYSQL_ROW mysqlFetchRow(mysql_handle *mh)
{
	if(!mh->pResult) 
	{ 
		return NULL; 
	} 
	
	return mysql_fetch_row(mh->pResult);
}

//�ͷ����ݿ�����ģ�� 
void mysqlFreeResult(mysql_handle *mh)
{
	if(mh->pResult) 
	{ 
		mysql_free_result(mh->pResult);
	}
}

//�ر�mysql
void mysqlClose(mysql_handle *mh)
{
	if(mh->pConn)
	{
		mysql_close(mh->pConn);
		mh->pConn = NULL;
	}
}
