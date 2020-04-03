/*
* Copyright (c) 2006
* All rights reserved.
* 
* �ļ����ƣ�api_mysql.h
* �ļ���ʶ�����ݿ�����ӿ�
* ��    �ߣ���־��(����)
*/

#ifndef _API_MYSQL_H
#define _API_MYSQL_H

#ifdef WIN32
#include "mysql/mysql.h"
#else
#include "mysql.h"
#endif

///////////////////////////

//�������ݿ����Ӳ����ṹ
typedef struct {
	MYSQL *pConn;//���ݿ����Ӿ��
	MYSQL_RES *pResult;//���ݼ����
	MYSQL_ROW pRow;//�������ݼ�
	int  isConnect;//�Ƿ��Ѿ����ӳɹ�
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

//���ݿ�����
db_cfg cfg;
db_cfg dbCfg;

/////
//��չ��ѯ�����ṹ
//���ĳ���ֶε�ֵ�Ƿ����ƥ�������
//�����ʹ�� �����Ϣ�γɵ��ֶ�
//��ѯ��ƥ��ǰ׺ֵ, ����ƥ�䣬�򲻻������ѯ����������
typedef struct sql_ext {
	int check_rec;//1:check 0:not check
	int col_num;//column no, from 1 start
	int fix_val;//will be fixed int val
}sql_ext;


/////////////////////////

///��¼����
struct listNode{ 
   apr_table_t* col_list;
   struct listNode *nextPtr; 
}; 

typedef struct listNode LISTNODE; 
typedef LISTNODE * DbRecordList; 


////////MYSQLԭʼ�ӿ�//////////////////////////////////////


//���ݿ�����
//����ֵ: 1�ɹ� 0ʧ��
int mysqlConnect(db_cfg cfg, mysql_handle* mh);
int mysqlSimpConnect(apr_pool_t *p, db_cfg cfg, mysql_handle* mh, char **errBuf);

//SQL���ִ��
//����ֵ: 1�ɹ� 0ʧ��
int mysqlQuery(apr_pool_t *p, mysql_handle* mh, const char *sql, char **errBuf);
int mysqlSimpQuery(apr_pool_t *p, mysql_handle* mh, const char *sql, char **errBuf);

//�洢��¼��ģ��
int mysqlStoreResult(mysql_handle* mh);

//��ý����ģ��
int mysqlNumRows(mysql_handle* mh);

//��¼����ģ��
MYSQL_ROW mysqlFetchRow(mysql_handle* mh);

//���ϵ��ֶ���
int mysqlFetchCol(mysql_handle* mh);

//�ͷ����ݿ�����ģ�� 
void mysqlFreeResult(mysql_handle* mh);

//�ر�mysql
void mysqlClose(mysql_handle* mh);

////////////////////////////////////////////////////////////


//��ʼ�������浥����¼�ڴ�ռ�
DbRecordList recordList_new(request_rec* r, DbRecordList sPtr, apr_table_t* rec_list);

/////////////////////////

//���ݿ��ʼ��
//�ɹ�1 ʧ�ܷ���0
int dbInit(apr_pool_t* p, db_cfg cfg, char** err_buf);

//��ִ�в��롢���¡�ɾ��SQL����
//�ɹ�1 ʧ�ܷ���0
int dbQuery(apr_pool_t* p, db_cfg cfg, const char* sql, char** err_buf);

int dbQueryTest(request_rec* r, db_cfg cfg, const char* sql, char** err_buf);


//��ý����
//�ɹ�����ƥ������ ʧ�ܷ���0
int dbGetRecords(request_rec* r, db_cfg cfg, const char* sql,
				 DbRecordList* sPtr, char** err_buf);


//��չ��ѯ����ý����
int dbGetRecordsExtend(request_rec* r, db_cfg cfg, const char* sql,
					   sql_ext sqex, DbRecordList* sPtr, char** err_buf);

////////////////////////////////

//060409 ��ѯkey=value��ʽ�����ݵ�table
//�ɹ�����ƥ������ ʧ�ܷ���0
int dbGetHashRecords(request_rec* r, db_cfg cfg, const char* sql,
					 		apr_table_t** suitHashRec, char** err_buf);


//�ָ����¼�����ֶε�table
int recordSplit(request_rec* r, char* signal_record, apr_table_t** col_list);

#ifdef __cplusplus
}
#endif

#endif	/* !_API_MYSQL_H */
