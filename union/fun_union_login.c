/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_union_login.c
* file desc£ºmember login funcs
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
#include "apache_cookie.h"

#include "fun_cfg.h"
#include "tpllib.h"
#include "fun_public.h"
#include "fun_union_login.h"

//
#define UNION_LOGIN_TPL "af_login.html"


//user login form
void member_login(request_rec* r, af_app_config* uacfg, apr_table_t* param_list)
{
	tpl_t *my_tpl;
	char* tpl_file = 0;
	char* content = 0;
	char* url = 0;//return url

	//show page header
	output_pageHeader(r, uacfg, "Login",  ucocfg);

	//tpl file init
	tpl_file_init(r, uacfg, &my_tpl, UNION_TPL_DIR, UNION_LOGIN_TPL);

	//set general variable
	gen_pub_varible_set(r, uacfg, &my_tpl);

	//get return url
	url = (char*)ap_table_get(param_list, "2");

	tpl_set_field_fmt_global(my_tpl, "return_url", "%s", url ? url : "");
	
	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);

	//ouput page bottom
	output_pageBottom(r, uacfg);
}


//user login check
int member_login_check(request_rec* r, af_app_config* uacfg,
					   ApacheRequest* req)
{
	const char* uname = 0;
	const char* upasswd = 0;
	const char* returnurl = 0;

	const char* orig_pwd = 0;
	const char* enc_pwd = 0;

	int ret = 0;
	int i = 0;
	
	char defRetUrl[MEDIM_MEM_SIZE] = {0};
	char locationUrl[MEDIM_MEM_SIZE] = {0};
	char cookieList[MEDIM_MEM_SIZE] = {0};
	char cookieListEnc[BIG_MEM_SIZE] = {0};

	unionBasicInfo ubi = {0};//user basic info

	//get POST param
	uname = ApacheRequest_param(req, "uname");
	upasswd = ApacheRequest_param(req, "upwd");
	returnurl = ApacheRequest_param(req, "url");

	snprintf(defRetUrl, sizeof(defRetUrl) - 1, "%s/union", uacfg->appRunUrl);
	snprintf(locationUrl, sizeof(locationUrl) - 1, "%s/union/login", uacfg->appRunUrl);

	if(!uname || !(*uname))
	{
		//error param
		output_msgPage(r, uacfg, ucocfg, "need user name", locationUrl, 0);
		return OK;
	}
	
	if(!upasswd || !(*upasswd))
	{
		//error param
		output_msgPage(r, uacfg, ucocfg, "need password", locationUrl, 0);
		return OK;
	}

	//login check by mysql mode
	ret = member_author_by_mysql(r, uacfg, uname, upasswd, &ubi);

	if(!ret)
	{
		output_msgPage(r, uacfg, ucocfg, "user name or password error", locationUrl, 0);
		return OK;
	}

	//format cookie list, uid|unick|uemail
	snprintf(cookieList, sizeof(cookieList) - 1, "%s|%s|%s|%s", ubi.uid, ubi.nick, uname, ubi.type);

	//encode cookie list
	apr_base64_encode(cookieListEnc, (char*)cookieList, strlen(cookieList));

	//set cookie to client 
	ApacheCookie *apc_cookie_list = ApacheCookie_new(r, 
					     "-name", uacfg->appCookieName ? uacfg->appCookieName : DEF_COOKIE_NAME, 
					     "-value", cookieListEnc,
						 "-expires", "+1M",
					     NULL);
	
	ApacheCookie_bake(apc_cookie_list);

	//relocation page to current url
	output_msgPage(r, uacfg, ucocfg, "login success...",   (char*)(returnurl ? returnurl : defRetUrl), 1);
	

	return OK;
}



//author by mysql
//1:ok 0:failed
int member_author_by_mysql(request_rec* r, af_app_config* uacfg,
						   const char* uname, const char* upwd,
						   unionBasicInfo *ubi)
{
	char sql_str[MEDIM_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	const char enc_pwd[MEDIM_MEM_SIZE] = {0};
	const char* tmp_passwd = 0;

	//email convert to lower
	ap_str_tolower((char*)uname);

	//password conver to lower
	ap_str_tolower((char*)upwd);

	//encode password	
	apr_base64_encode(enc_pwd, (char*)upwd, strlen(upwd));

	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	if(!global_af_handler[rand_mh].pConn)
	{
		mysqlSimpConnect(r->pool, af_db_cfg, &global_af_handler[rand_mh], &errBuf);
	}

	//get suit email and password
	//mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], "SET NAMES 'GB2312'", &errBuf);

	memset(sql_str, 0, sizeof(sql_str));
	snprintf(sql_str, sizeof(sql_str) - 1, "select passwd, id, name, userType from af_member where uname = '%s' limit 1", uname);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	if(!ret)
	{
		return 0;
	}

	rec_num = mysqlNumRows(&global_af_handler[rand_mh]);

	if(rec_num <= 0)
	{
		return 0;
	}

	global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]);

	tmp_passwd	= global_af_handler[rand_mh].pRow[0];

	//store reback data
	sprintf(ubi->uid, "%s", global_af_handler[rand_mh].pRow[1]);
	sprintf(ubi->nick, "%s", global_af_handler[rand_mh].pRow[2]);
	sprintf(ubi->type, "%s", global_af_handler[rand_mh].pRow[3]);


	if(tmp_passwd && strcmp(tmp_passwd, enc_pwd))
	{		
		//free result
		mysqlFreeResult(&global_af_handler[rand_mh]);

		return 0;
	}
	
	//free result
	mysqlFreeResult(&global_af_handler[rand_mh]);

	return 1;
}


//exit
void member_exit(request_rec* r, af_app_config* uacfg)
{
	//set cookie to client 
	ApacheCookie *apc_cookie_list = ApacheCookie_new(r, 
					     "-name", uacfg->appCookieName ? uacfg->appCookieName : DEF_COOKIE_NAME, 
					     "-value", NULL,
						  "-expires", NULL,
					     NULL);
	
	ApacheCookie_bake(apc_cookie_list);

	//relocation page to current url
	output_msgPage(r, uacfg, ucocfg, "exit success...",   uacfg->appRunUrl, 1);
}
