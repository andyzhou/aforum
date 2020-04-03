/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_union_reg.c
* file desc£ºmember registe funcs
* author:AndyZhou<diudiu8848@163.com>
*/

#include <stdio.h>
#include <string.h>

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

#include "apache_request.h"

#include "fun_cfg.h"
#include "tpllib.h"
#include "fun_public.h"
#include "fun_union_reg.h"

//
#define UNION_REG_TPL "af_reg.html"

//get new user hash table value
unsigned long _gen_new_member_hash_id(request_rec* r, const char* uemail, int max_val);

void member_reg(request_rec* r, af_app_config* uacfg)
{
	tpl_t *my_tpl;
	char* tpl_file = 0;
	char* content = 0;

	struct tm *now_time;
	time_t nowtime;
	char cur_year[6] = {0};
	char tmp_str[32] = {0};

	unsigned long u_hash_id = 0;
	int i = 0;

	int yearBegin = 0;
	int yearEnd	  = 0;

	//get current year value
	nowtime = time(NULL);
	now_time = localtime(&nowtime);
	strftime(cur_year, 4, "%Y", now_time);
	
	//show page header
	output_pageHeader(r, uacfg, "Regist",   ucocfg);


	//tpl file init
	tpl_file_init(r, uacfg, &my_tpl, UNION_TPL_DIR, UNION_REG_TPL);

	//set general variable
	gen_pub_varible_set(r, uacfg, &my_tpl);


	/*
	//birth begin and end year
	yearBegin = atoi(cur_year) - 70;
	yearEnd	  = atoi(cur_year) - 15;
	
	for(i = yearBegin; i <= yearEnd; i++)
	{
		memset(tmp_str, 0, sizeof(tmp_str));
		sprintf(tmp_str, "%d", i);

		tplSetVar(my_tpl, "b_year", tmp_str);
		tplParse(r, my_tpl, "byearList");
	}


	//birth month
	for(i = 1; i <= 12; i++)
	{
		memset(tmp_str, 0, sizeof(tmp_str));
		sprintf(tmp_str, "%d", i);

		tplSetVar(my_tpl, "b_mon", tmp_str);
		tplParse(r, my_tpl, "bmonList");
	}


	//birth day
	for(i = 1; i <= 31; i++)
	{
		memset(tmp_str, 0, sizeof(tmp_str));
		sprintf(tmp_str, "%d", i);

		tplSetVar(my_tpl, "b_day", tmp_str);
		tplParse(r, my_tpl, "bdayList");
	}
	*/

	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);

	//ouput page bottom
	output_pageBottom(r, uacfg);

}


//new user check
//ret -1:error param 0:failed 1:success
int member_check_new(request_rec *r, char* uname, af_app_config* uacfg)
{
	char* newEmail = NULL;

	char sql_str[MEDIM_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;	

	if(!uname || !(*uname))
	{
		return -1;
	}

	//string convert to lower
	ap_str_tolower((char*)uname);

	//by mysql mode
	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	if(!global_af_handler[rand_mh].pConn)
	{
		mysqlSimpConnect(r->pool, af_db_cfg, &global_af_handler[rand_mh], &errBuf);
	}

	memset(sql_str, 0, sizeof(sql_str));
	sprintf(sql_str, "select uname from af_member where uname = '%s' limit 1", uname);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	if(!ret)
	{
		return 0;
	}

	rec_num = mysqlNumRows(&global_af_handler[rand_mh]);

	if(rec_num > 0)
	{
		return 0;
	}

	//email not registed
	return 1;
}


//check user is registed
void member_check_valid(request_rec* r, af_app_config* uacfg, apr_table_t* param_list)
{
	int ret = 0;
	char* newUName = NULL;

	output_header(r);

	//get new user name
	newUName = (char*)ap_table_get(param_list, "3");
	
	if(!newUName || !(*newUName))
	{
		output_simplePage(r, uacfg, "Need user name");
		return;
	}

	ret = member_check_new(r, newUName, uacfg);

	if(ret == 1)
	{
		//user name not registed
		output_simplePage(r, uacfg, "user name is ok.");
	}
	else
	{
		//email has registed
		output_simplePage(r, uacfg, "user name is registed.");
	}
}



//user reg save
void member_reg_save(request_rec* r, af_app_config* uacfg,
					 ApacheRequest* req)
{
	char sql_str[BIG_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	const char* uname = 0;
	const char* upasswd = 0;
	const char* upasswd2 = 0;
	const char* unick = 0;

	char locationUrl[MEDIM_MEM_SIZE] = {0};
	char errMsg[MEDIM_MEM_SIZE] = {0};
	char enc_pwd[MEDIM_MEM_SIZE] = {0};

	int has_same_uname = 0;
	int has_same_nick = 0;
	int hash_table = 0;
	int i = 0;

	const char* tmp_uname = 0;
	const char* tmp_nick = 0;
	const char* new_user_id = 0;
	const char* enc_email = 0;
	char curDate[SMALL_MEM_SIZE] = {0};

	//get current time
	struct tm *now_time;
	time_t nowtime;

	nowtime = time(NULL);
	now_time = localtime(&nowtime);
	strftime(curDate, 20, "%Y-%m-%d", now_time);

	//get input param 
	uname		= ApacheRequest_param(req, "userName");
	upasswd		= ApacheRequest_param(req, "userPwd");
	unick		= ApacheRequest_param(req, "userNick");

	sprintf(locationUrl, "%s/union/reg", uacfg->appRunUrl);

	if(!uname || !(*uname))
	{
		//error param
		output_msgPage(r, uacfg, ucocfg, "need user name", locationUrl, 0);
		return;
	}
	
	if(!upasswd || !(*upasswd))
	{
		//error param
		output_msgPage(r, uacfg, ucocfg, "need password", locationUrl, 0);
		return;
	}

	if(!unick || !(*unick))
	{
		//error param
		output_msgPage(r, uacfg, ucocfg, "need nick", locationUrl, 0);
		return;
	}

	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	if(!global_af_handler[rand_mh].pConn)
	{
		mysqlSimpConnect(r->pool, af_db_cfg, &global_af_handler[rand_mh], &errBuf);
	}

	//check user name or nickname is registed
	memset(sql_str, 0, sizeof(sql_str));
	sprintf(sql_str, "select uname, name from af_member where uname = '%s' or name = '%s'",
					  uname, unick);

	//mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], "SET NAMES 'GB2312'", &errBuf);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	if(!ret)
	{
		//ap_set_content_type(r, "text/html");
		//ap_rprintf(r, "<h1>%s, %s</h1>", sql_str, errBuf);

		//query failed
		output_msgPage(r, uacfg, ucocfg, "query failed, try again.", locationUrl, 0);
		return;
	}

	rec_num = mysqlNumRows(&global_af_handler[rand_mh]);

	if(rec_num > 0)
	{
		while(global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]))
		{
			tmp_uname	= global_af_handler[rand_mh].pRow[0];
			tmp_nick	= global_af_handler[rand_mh].pRow[1];
			
			//check email¡¢nickname is same record
			if(tmp_uname && !strcmp(tmp_uname, uname))
			{
				//user name is same
				has_same_uname = 1;
				break;
			}

			if(tmp_nick && !strcmp(tmp_nick, unick))
			{
				//nick is same
				has_same_nick = 1;
				break;
			}
		}
		
		//free result 
		mysqlFreeResult(&global_af_handler[rand_mh]);
		
		if(has_same_uname)
		{
			//user name is registed
			output_msgPage(r, uacfg, ucocfg, "user name has registed", locationUrl, 0);
			return;
		}

		
		if(has_same_nick)
		{
			//nick is registed
			output_msgPage(r, uacfg, ucocfg, "nick has registed", locationUrl, 0);
			return;
		}
	}

	//save new user by mysql mode //////////////////////

	//email convert to lower
	ap_str_tolower((char*)uname);

	//password convert to lower
	ap_str_tolower((char*)upasswd);

	//encode password
	//enc_pwd = ap_uuencode(r->pool, (char*)upasswd);
	apr_base64_encode(enc_pwd, (char*)upasswd, strlen(upasswd));

	//registe new user
	memset(sql_str, 0, sizeof(sql_str));
	sprintf(sql_str, "insert into af_member(uname, passwd, name, regdate) \
					  values('%s', '%s', '%s', '%s')", uname, enc_pwd, unick, curDate);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	if(!ret)
	{
		//insert failed
		output_msgPage(r, uacfg, ucocfg, "regist failed, try again.", locationUrl, 0);
		return;
	}

	//registe success, relocation to login page.
	memset(locationUrl, 0, sizeof(locationUrl));
	sprintf(locationUrl, "%s/union/login", uacfg->appRunUrl);
	output_msgPage(r, uacfg, ucocfg, "regist success!!!", locationUrl, 0);
}



/*
//get new user hash table value
unsigned long _gen_new_member_hash_id(request_rec* r, const char* uemail, int max_val)
{
	static unsigned long	ulZicId;
	static unsigned int	uiChar;
	int new_max_val = 0;

	if(max_val <= 0)
	{
		return 0;
	}

	ulZicId = 8;
	while (uiChar = (unsigned int)*uemail++)
	{
		//ap_rprintf(r, "<h1>uiChar:%d</h1>", uiChar);

		// ulZicId * 33 + uiChar //
		ulZicId = ((ulZicId << 3) + ulZicId) + uiChar;
	}

	return (int)((ulZicId >> 6) & max_val);
}

*/



