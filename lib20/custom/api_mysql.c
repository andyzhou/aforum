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

#include "apr_buckets.h"
#include "apr_strings.h"
#include "apr_general.h"
#include "apr_lib.h"
#include "apr_compat.h"
#include "apr_pools.h"


#ifdef WIN32
#include "mysql/mysql.h"
#else
#include "mysql.h"
#endif

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


//���ݿ��ʼ��
//�ɹ�1 ʧ�ܷ���0
int dbInit(apr_pool_t* p, db_cfg cfg, char** err_buf)
{
	int ret = 0;
	//int rand_mysql_service_no = 0;

	//����������ֵ 0 - MYSQL_SERVICE_NUM
	//rand_mysql_service_no = get_rand_num(MYSQL_SERVICE_NUM);

	if(cfg.db_no <= 0)
	{
		cfg.db_no = 1;
	}

	switch(cfg.db_no)
	{
	case 1:
		//����zic_global
		sprintf(cfg.db_name, "zic_global");
		if(!zic_global_mhandler[cfg.rand_service_no].pConn)
		{
			ret = mysqlConnect(cfg, &(zic_global_mhandler[cfg.rand_service_no]));

			if(!ret)
			{
				return 0;
			}
		}
		break;

	case 2:
		//����zic_u
		sprintf(cfg.db_name, "zic_u");
		if(!zic_u_mhandler[cfg.rand_service_no].pConn)
		{
			ret = mysqlConnect(cfg, &zic_u_mhandler[cfg.rand_service_no]);

			if(!ret)
			{
				return 0;
			}
		}
		break;
		

	case 3:
		//����zic_g
		sprintf(cfg.db_name, "zic_g");
		if(!zic_g_mhandler[cfg.rand_service_no].pConn)
		{
			ret = mysqlConnect(cfg, &zic_g_mhandler[cfg.rand_service_no]);

			if(!ret)
			{
				return 0;
			}
		}
		break;

	default:
		//����zic_global
		sprintf(cfg.db_name, "zic_global");
		if(!zic_global_mhandler[cfg.rand_service_no].pConn)
		{
			ret = mysqlConnect(cfg, &zic_global_mhandler[cfg.rand_service_no]);

			if(!ret)
			{
				return 0;
			}
		}
		break;
	}
	
	/*
	if(!mydb)
	{
		//�������ݿ�
		ret = mysqlConnect(p, cfg.db_server, cfg.db_user, cfg.db_pwd,
						   cfg.db_name, cfg.db_port, err_buf);
		if(!ret)
		{
			return 0;
		}
	}
	*/
	
	return 1;
}


//��ִ�в��롢���¡�ɾ��SQL����
//������Ч����-1 �ɹ�����1 ʧ�ܷ���0
//�ӿ�����:public
int dbQuery(apr_pool_t* p, db_cfg cfg, const char* sql, char** err_buf)
{
	return 0;
}

int dbQueryTest(request_rec* r, db_cfg cfg, const char* sql, char** err_buf)
{
	int sql_type = 0;
	int ret = 0;

	mysql_handle* local_mh;

	
	//�ж�sql����Ƿ���Ч
	//��ִ��insert��update��delete����
	sql_type = sql_str_type(sql);

	ap_rprintf(r, "<h1>sql:%s</h1>", sql);
	
	/*
	if(sql_type < 2)
	{
		return -1;
	}
	*/

	//���������
	cfg.rand_service_no = get_rand_num(MYSQL_SERVICE_NUM);
	
	if(cfg.db_no <= 0)
	{
		cfg.db_no = 1;
	}

	ap_rprintf(r, "<h1>rand_service_no:%d</h1>", cfg.rand_service_no);

	ap_rprintf(r, "<h1>db_serv:%s, db_user:%s, db_pwd:%s, db_name:%s, db_port:%d</h1>",
					cfg.db_server, cfg.db_user, cfg.db_pwd, cfg.db_name, cfg.db_port);

	//�жϵ�ǰdb���Ӿ���Ƿ����
	//�����������
	switch(cfg.db_no)
	{
	case 1:
		//zic_global
		if(!zic_global_mhandler[cfg.rand_service_no].pConn)
		{
			ap_rprintf(r, "<h1>init_a</h1>");

			ret = dbInit(r->pool, cfg, err_buf);

			if(!ret)
			{
				return 0;
			}
		}

		local_mh = &(zic_global_mhandler[cfg.rand_service_no]);
		break;

	case 2:
		//zic_u
		if(!zic_u_mhandler[cfg.rand_service_no].pConn)
		{
			ap_rprintf(r, "<h1>init_b</h1>");

			ret = dbInit(r->pool, cfg, err_buf);

			if(!ret)
			{
				return 0;
			}
		}
		local_mh = &(zic_u_mhandler[cfg.rand_service_no]);

		break;
		

	case 3:
		//zic_g
		if(!zic_g_mhandler[cfg.rand_service_no].pConn)
		{
			ap_rprintf(r, "<h1>init_c</h1>");

			ret = dbInit(r->pool, cfg, err_buf);

			if(!ret)
			{
				return 0;
			}
		}

		local_mh = &(zic_g_mhandler[cfg.rand_service_no]);
		break;

	default:
		//zic_global
		if(!zic_global_mhandler[cfg.rand_service_no].pConn)
		{
			ap_rprintf(r, "<h1>init_a</h1>");

			ret = dbInit(r->pool, cfg, err_buf);

			if(!ret)
			{
				return 0;
			}
		}

		local_mh = &(zic_global_mhandler[cfg.rand_service_no]);
		break;
	}

	/*
	if(!mydb)
	{
		//�������ݿ�
		ret = dbInit(p, cfg, err_buf);
		
		if(!ret)
		{
			return 0;
		}
	}
	*/

	ap_rprintf(r, "<h1>query begin..</h1>");
	
	//ִ�в��롢���¡�ɾ������
	ret = mysqlQuery(r->pool, local_mh, sql, err_buf);
	
	if(!ret)
	{
		return 0;
	}

	ap_rprintf(r, "<h1>query ok ..</h1>");
	
	return 1;
}



//��ý����
//�ɹ�����ƥ������ ʧ�ܷ���0
//�ӿ�����:public
int dbGetRecords(request_rec* r, db_cfg cfg, const char* sql,
				 DbRecordList* sPtr, char** err_buf)
{
	int i = 0;
	int sql_type = 0;
	int ret = 0;
	int col_num = 0;
	int record_num = 0;
	int num = 0;
	
	mysql_handle* local_mh;

	//char* err_buf = 0;
	char key_str[10] = {0};
	apr_table_t* tmp_table;
	
	//sql�����ж�
	sql_type = sql_str_type(sql);

	//ap_rprintf(r, "<h1>sql:%s</h1>", sql);
	
	if(sql_type != 1)
	{
		return 0;
	}
	
	/*
	if(!mydb)
	{
		//�������ݿ�
		ret = mysqlConnect(r->pool, (char*)cfg.db_server, (char*)cfg.db_user, 
						  (char*)cfg.db_pwd, (char*)cfg.db_name, cfg.db_port, err_buf);
		
		if(!ret)
		{
			return 0;
		}
	}
	*/

	//���������
	cfg.rand_service_no = get_rand_num(MYSQL_SERVICE_NUM);
	
	if(cfg.db_no <= 0)
	{
		cfg.db_no = 1;
	}

	//�жϵ�ǰdb���Ӿ���Ƿ����
	//�����������
	switch(cfg.db_no)
	{
	case 1:
		//zic_global
		if(!zic_global_mhandler[cfg.rand_service_no].pConn)
		{
			ret = dbInit(r->pool, cfg, err_buf);

			if(!ret)
			{
				return 0;
			}
		}

		local_mh = &(zic_global_mhandler[cfg.rand_service_no]);
		break;

	case 2:
		//zic_u
		if(!zic_u_mhandler[cfg.rand_service_no].pConn)
		{
			ret = dbInit(r->pool, cfg, err_buf);

			if(!ret)
			{
				return 0;
			}
		}
		local_mh = &(zic_u_mhandler[cfg.rand_service_no]);

		break;
		

	case 3:
		//zic_g
		if(!zic_g_mhandler[cfg.rand_service_no].pConn)
		{
			ret = dbInit(r->pool, cfg, err_buf);

			if(!ret)
			{
				return 0;
			}
		}

		local_mh = &(zic_g_mhandler[cfg.rand_service_no]);
		break;

	default:
		//zic_global
		if(!zic_global_mhandler[cfg.rand_service_no].pConn)
		{
			ret = dbInit(r->pool, cfg, err_buf);

			if(!ret)
			{
				return 0;
			}
		}

		local_mh = &(zic_global_mhandler[cfg.rand_service_no]);
		break;
	}
	
	//��ѯ
	//ret = mysqlQuery(r->pool, sql, err_buf);

	ret = mysqlQuery(r->pool, local_mh, sql, err_buf);
	
	if(!ret)
	{
		return 0;
	}
	
	//�洢ƥ����
	ret = mysqlStoreResult(local_mh);
	
	if(!ret)
	{
		return 0;
	}
	
	num = 0;
	
	num = mysql_num_rows(local_mh->pResult);
	
	//ap_rprintf(r, "<h1>num:%d</h1>", num);	
		
	tmp_table = ap_make_table(r->pool, 1);

	
	while(myrow = mysqlFetchRow(local_mh))
	{
		col_num = mysqlFetchCol(local_mh);
		
		//ap_rprintf(r, "<h1>col_num:%d</h1>", col_num);
		
		if(tmp_table && !ap_is_empty_table(tmp_table))
		{
			ap_clear_table(tmp_table);
		}
		
		for(i = 0; i < col_num; i++)
		{
			//col_info = mysql_fetch_field_direct(res, i);
			memset(key_str, 0, 10);
			sprintf(key_str, "%d", i+ 1);
			
			//ap_rprintf(r, "<h1>col:%d, val:%s</h1>", i,  myrow[i] ? myrow[i] : 0);
			
			//ap_table_add(tmp_table, key_str,  (myrow[i] && !strcmp(myrow[i], "(null)")) ? myrow[i] : 0);
			
			//old
			//ap_table_add(tmp_table, key_str,  myrow[i] ? myrow[i] : 0);

			ap_table_add(tmp_table, key_str, local_mh->pRow[i] ? local_mh->pRow[i] : 0);
			
			//ap_table_add(tmp_table, "name", "diudiu");
			//ap_table_add(tmp_table, "sex", "male");
		}
		
		*sPtr = recordList_new(r, *sPtr, tmp_table);
		record_num++;
	}
	
	return record_num;
}


//��չ��ѯ����ý����
int dbGetRecordsExtend(request_rec* r, db_cfg cfg, const char* sql,
					   sql_ext sqex, DbRecordList* sPtr, char** err_buf)
{
	int i = 0;
	int sql_type = 0;
	int ret = 0;
	int col_num = 0;
	int record_num = 0;
	int num = 0;
	int k = 0;
	
	//char* err_buf = 0;
	char key_str[10] = {0};
	apr_table_t* tmp_table;

	char tmp_ch_str[3] = {0};
	char tmp_int_str[20] = {0};
	char tmp_str[32] = {0};

	mysql_handle* local_mh;
	
	//sql�����ж�
	sql_type = sql_str_type(sql);

	//ap_rprintf(r, "<h1>sql:%s</h1>", sql);
	
	if(sql_type != 1)
	{
		return 0;
	}

	//���������
	cfg.rand_service_no = get_rand_num(MYSQL_SERVICE_NUM);
	
	if(cfg.db_no <= 0)
	{
		cfg.db_no = 1;
	}

	//�жϵ�ǰdb���Ӿ���Ƿ����
	//�����������
	switch(cfg.db_no)
	{
	case 1:
		//zic_global
		if(!zic_global_mhandler[cfg.rand_service_no].pConn)
		{
			ret = dbInit(r->pool, cfg, err_buf);

			if(!ret)
			{
				return 0;
			}
		}

		local_mh = &(zic_global_mhandler[cfg.rand_service_no]);
		break;

	case 2:
		//zic_u
		if(!zic_u_mhandler[cfg.rand_service_no].pConn)
		{
			ret = dbInit(r->pool, cfg, err_buf);

			if(!ret)
			{
				return 0;
			}
		}
		local_mh = &(zic_u_mhandler[cfg.rand_service_no]);

		break;
		

	case 3:
		//zic_g
		if(!zic_g_mhandler[cfg.rand_service_no].pConn)
		{
			ret = dbInit(r->pool, cfg, err_buf);

			if(!ret)
			{
				return 0;
			}
		}

		local_mh = &(zic_g_mhandler[cfg.rand_service_no]);
		break;

	default:
		//zic_global
		if(!zic_global_mhandler[cfg.rand_service_no].pConn)
		{
			ret = dbInit(r->pool, cfg, err_buf);

			if(!ret)
			{
				return 0;
			}
		}

		local_mh = &(zic_global_mhandler[cfg.rand_service_no]);
		break;
	}
	
	/*
	if(!mydb)
	{
		//�������ݿ�
		ret = mysqlConnect(r->pool, (char*)cfg.db_server, (char*)cfg.db_user, 
						  (char*)cfg.db_pwd, (char*)cfg.db_name, cfg.db_port, err_buf);
		
		if(!ret)
		{
			return 0;
		}
	}
	*/
	
	ret = mysqlQuery(r->pool, local_mh, sql, err_buf);
	
	if(!ret)
	{
		return 0;
	}
	
	//�洢ƥ����
	ret = mysqlStoreResult(local_mh);
	
	if(!ret)
	{
		return 0;
	}
	
	
	num = mysql_num_rows(local_mh->pResult);
	
	//ap_rprintf(r, "<h1>num:%d</h1>", num);	

		//ap_rprintf(r, "<h1>sqex.check_rec:%d, sqex.fix_val:%d, sqex.col_num:%d</h1>",
	//						sqex.check_rec, sqex.fix_val, sqex.col_num);
		
	tmp_table = ap_make_table(r->pool, 1);

	
	while(myrow = mysqlFetchRow(local_mh))
	{
		col_num = mysqlFetchCol(local_mh);
		
		//ap_rprintf(r, "<h1>col_num:%d</h1>", col_num);
		
		if(tmp_table && !ap_is_empty_table(tmp_table))
		{
			ap_clear_table(tmp_table);
		}
		
		//���м��ָ��
		if(sqex.check_rec && sqex.fix_val > 0 && sqex.col_num > 0 
		   && sqex.col_num <= col_num)
		{

			memset(tmp_int_str, 0, sizeof(tmp_int_str));
			memset(tmp_str, 0, sizeof(tmp_str));
			
			sprintf(tmp_int_str, "%d", sqex.fix_val);
			
			//ap_rprintf(r, "<h1>tmp_int_str:%s</h1>", tmp_int_str);
			
			for(k = 0; k < strlen(tmp_int_str); k++)
			{
				memset(tmp_ch_str, 0, sizeof(tmp_ch_str));
				sprintf(tmp_ch_str, "%c", myrow[sqex.col_num - 1][k]);
			
				//ap_rprintf(r, "<h1>ss:%c</h1>", myrow[sqex.col_num - 1][k]);

				if(k == 0)
				{
					strcpy(tmp_str, tmp_ch_str);
				}
				else
				{
					strcat(tmp_str, tmp_ch_str);
				}
			}

			//ap_rprintf(r, "<h1>tmp_int_str:%s, tmp_str:%s</h1>", tmp_int_str, tmp_str);

			if(atoi(tmp_int_str) != atoi(tmp_str)/* || strlen(myrow[sqex.col_num - 1]) != (strlen(tmp_int_str) + 10)*/)
			{
				//��ƥ�䣬�����Ѿ�ƥ��ļ�¼
				return record_num;
			}
		}
		
		for(i = 0; i < col_num; i++)
		{
			//col_info = mysql_fetch_field_direct(res, i);
			memset(key_str, 0, 10);
			sprintf(key_str, "%d", i+ 1);
			
			//ap_rprintf(r, "<h1>col:%d, val:%s</h1>", i,  myrow[i] ? myrow[i] : 0);
			
			//ap_table_add(tmp_table, key_str,  (myrow[i] && !strcmp(myrow[i], "(null)")) ? myrow[i] : 0);
			
			//old
			//ap_table_add(tmp_table, key_str,  myrow[i] ? myrow[i] : 0);
			ap_table_add(tmp_table, key_str, local_mh->pRow[i] ? local_mh->pRow[i] : 0);
			
			//ap_table_add(tmp_table, "name", "diudiu");
			//ap_table_add(tmp_table, "sex", "male");
		}
		
		*sPtr = recordList_new(r, *sPtr, tmp_table);
		record_num++;
	}
	
	return record_num;
}


//060409 ��ѯkey=value��ʽ�����ݵ�table
//�ɹ�����ƥ������ ʧ�ܷ���0
int dbGetHashRecords(request_rec* r, db_cfg cfg, const char* sql,
					 apr_table_t** suitHashRec, char** err_buf)
{
	int i = 0;
	int sql_type = 0;
	int ret = 0;
	int col_num = 0;
	int record_num = 0;
	int tmp_col_num = 0;
	
	//char* err_buf = 0;
	char key_str[64] = {0};
	apr_table_t* tmp_table;
	
	//sql�����ж�
	sql_type = sql_str_type(sql);
	
	if(sql_type != 1)
	{
		return 0;
	}
	
	/*
	if(!mydb)
	{
		//�������ݿ�
		ret = mysqlConnect(r->pool, (char*)cfg.db_server, (char*)cfg.db_user, 
						  (char*)cfg.db_pwd, (char*)cfg.db_name, cfg.db_port, err_buf);
		
		if(!ret)
		{
			return 0;
		}
	}
	
	ret = mysqlQuery(r->pool, sql, err_buf);
	
	if(!ret)
	{
		return 0;
	}
	
	//�洢ƥ����
	ret = mysqlStoreResult();
	
	if(!ret)
	{
		return 0;
	}
	
	int num = 0;
	
	num = mysql_num_rows(res);
			
	*suitHashRec = ap_make_table(r->pool, 1);
	tmp_table = ap_make_table(r->pool, 1);
	
	while(myrow = mysqlFetchRow())
	{
		col_num = mysqlFetchCol();

		//ֻȡǰ�����ֶ�
		tmp_col_num = col_num > 2 ? 2 : col_num;

		memset(key_str, 0, sizeof(key_str));
		sprintf(key_str, "%s", myrow[0]);

		//ap_rprintf(r, "<h1>key:%s, val:%s</h1>", key_str, myrow[1]); 

		ap_table_add(*suitHashRec, key_str, myrow[1] ? myrow[1] : 0);
		//ap_table_add(tmp_table, key_str, myrow[1]);
		
		
		//*sPtr = recordList_new(r, *sPtr, tmp_table);
		record_num++;
	}

	//ap_rprintf(r, "<h1>suit:%s</h1>", ap_apr_table_t_get(tmp_table, "40"));
	//ap_rprintf(r, "<h1>suit1:%s</h1>", ap_apr_table_t_get(*suitHashRec, "40"));
	*/
	
	return record_num;
}


//�ָ����¼�����ֶε�table
//�Զ��ŷָ�, �洢��col_list
int recordSplit(request_rec* r, char* signal_record, apr_table_t** col_list)
{
	int col_num = 0;
	int record_len = 0;
	int tmp_len = 0;
	int n = 0;
	
	//�ָ��ַ�
	char tokstr[] = ",";
	
	char* tmp_str = 0;
	char* suit_col_str = 0;
	char key_str[10] = {0};
	
	if(!signal_record || !(*signal_record))
	{
		return col_num;
	}
	
	//��ʼ������洢table
	*col_list = ap_make_table(r->pool, 1);
	
	while (*signal_record != '\0')
	{
		suit_col_str = ap_getword_nc(r->pool, &signal_record, ',');
		
		if(!suit_col_str || !(*suit_col_str))
		{
			continue;
		}
		
		//������ֵ���浽table
		memset(key_str, 0, 10);
		sprintf(key_str, "%d", col_num);
		
		if(col_num > 0)
		{
			//ȥ���ڶ����ֶμ�֮���ֶεĵ�һ���ַ�����
			suit_col_str[0] = suit_col_str[1];
			suit_col_str++;
		}
		
		//���ֶ�ֵ����table
		ap_table_add(*col_list, key_str, suit_col_str);
		
		col_num++;
	}
	
	return col_num;
}

///////////////////////


//��������ӵ�
DbRecordList recordList_new(request_rec* r, DbRecordList sPtr, apr_table_t* rec_list) 
{ 
	if(sPtr != NULL)
	{
		// �ݹ飬�����Ľڵ��ϼ��� �ݡ� 
		sPtr->nextPtr = recordList_new(r, sPtr->nextPtr, rec_list); 
	}
    else 
    { 
		//// ע�⣬�ǽڵ�ĳߴ�, ����ת�� 
		//��Ԫ�ؼ����
        sPtr =(DbRecordList)ap_palloc(r->pool, sizeof(LISTNODE));
        sPtr =(DbRecordList)ap_pcalloc(r->pool, sizeof(LISTNODE));
        sPtr->nextPtr = NULL;
		
		sPtr->col_list = ap_make_table(r->pool, 0);
		//ap_table_add(sPtr->col_list, "name", "diudiu");
		//ap_table_add(sPtr->col_list, "age", "28");
		sPtr->col_list = (!ap_is_empty_table(rec_list)) ? ap_copy_table(r->pool, rec_list) : NULL;
		
    }

    return sPtr; 
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
int mysqlConnect(apr_pool_t *p, char *dbServer, char *dbUser, char *dbPwd, 
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


int mysqlSimpConnect(apr_pool_t *p, db_cfg cfg, mysql_handle* mh, char **errBuf)
{
	*errBuf = (char*)ap_palloc(p, 256);
	*errBuf = (char*)ap_pcalloc(p, 256);
	
	//��������
	if(!cfg.db_user || !strcmp(cfg.db_user, ""))
	{
		*errBuf = ap_psprintf(p, "%s", "param cfg.db_user is empty");
		return 0;
	}

	if(!cfg.db_pwd || !strcmp(cfg.db_pwd, ""))
	{
		*errBuf = ap_psprintf(p, "%s", "param cfg.db_pwd is empty");
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
		*errBuf = ap_psprintf(p, "%s", (char *)mysql_error(mydb));
		return 0;
	}

	if(mysql_real_connect(mh->pConn, cfg.db_server, cfg.db_user,	
		cfg.db_pwd, cfg.db_name, cfg.db_port, cfg.db_sock, 0))
	{
		mh->isConnect = 1;
		//���ӳɹ�
		return 1;
	}

#else

	if(mysql_real_connect(mh->pConn, cfg.db_server, cfg.db_user,
		cfg.db_pwd, cfg.db_port, NULL, 0))
	{
		mh->isConnect = 1;
		return 1;
	}
#endif

	//ap_rprintf(r, "<h1>error:%s</h1>", (char *)mysql_error(mh->pConn));
	*errBuf = ap_psprintf(p, "%s", (char *)mysql_error(mydb));
	
	return 0;
}


//SQL���ִ��
//����ֵ: 1�ɹ� 0ʧ��
int mysqlQuery(apr_pool_t *p, mysql_handle *mh, const char *sql, char **errBuf)
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



int mysqlSimpQuery(apr_pool_t *p, mysql_handle *mh, const char *sql, char **errBuf)
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
