/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_union_pwd.c
* file desc£ºmember change password
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

#include "fun_union_pwd.h"

#define UNION_PWD_EDIT_TPL "af_edit_pwd.html"

//show edit pwd form
void member_edit_pwd(request_rec* r, af_app_config* facfg)
{
	tpl_t *my_tpl;
	char* content = 0;

	char tmp_str[BIG_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	const char* photo = 0;
	int has_photo = 0;

	//show page header
	output_pageHeader(r, facfg, "Change Password",  ucocfg);


	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, UNION_TPL_DIR, UNION_PWD_EDIT_TPL);

	//set general variable
	gen_pub_varible_set(r, facfg, &my_tpl);

	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);

	//ouput page bottom
	output_pageBottom(r, facfg);
}


//save new pwd
void member_save_pwd(request_rec* r, af_app_config* facfg, ApacheRequest *req)
{
	char sql_str[BIG_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	char locationUrl[MEDIM_MEM_SIZE] = {0};
	char enc_old_pwd[MEDIM_MEM_SIZE] = {0};
	char enc_new_pwd[MEDIM_MEM_SIZE] = {0};

	const char* oldpwd = 0;
	const char* pwd1 = 0;
	const char* pwd2 = 0;

	const char* tmpPwd = 0;

	//get input param 
	oldpwd	= ApacheRequest_param(req, "oldpwd");
	pwd1		= ApacheRequest_param(req, "pwd1");
	pwd2		= ApacheRequest_param(req, "pwd2");

	sprintf(locationUrl, "%s/union/my/editpwd", facfg->appRunUrl);

	if(!oldpwd || !(*oldpwd))
	{
		//error param
		output_msgPage(r, facfg, ucocfg, "need old password.", locationUrl, 0);
		return;
	}
	
	if(!pwd1 || !(*pwd1))
	{
		//error param
		output_msgPage(r, facfg, ucocfg, "need new password", locationUrl, 0);
		return;
	}

	if(strcmp(pwd1, pwd2))
	{
		//error param
		output_msgPage(r, facfg, ucocfg, "two new passwords not suit", locationUrl, 0);
		return;
	}

	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	//get old password value
	memset(sql_str, 0, sizeof(sql_str));
	sprintf(sql_str, "select passwd from af_member where id = %d limit 1", ucocfg.uid);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	if(!ret)
	{
		//query failed
		output_msgPage(r, facfg, ucocfg, "query failed, try again", locationUrl, 0);
		return;
	}

	rec_num = mysqlNumRows(&global_af_handler[rand_mh]);

	if(rec_num <= 0)
	{
		output_msgPage(r, facfg, ucocfg, "invalid data, try again", locationUrl, 0);
		return;
	}

	//encode old password and new password
	apr_base64_encode(enc_old_pwd, (char*)oldpwd, strlen(oldpwd));
	apr_base64_encode(enc_new_pwd, (char*)pwd1, strlen(pwd1));

	global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]);

	tmpPwd = global_af_handler[rand_mh].pRow[0];

	//check old password is suit?
	if(strcmp(enc_old_pwd, tmpPwd))
	{
		//free result 
		mysqlFreeResult(&global_af_handler[rand_mh]);

		output_msgPage(r, facfg, ucocfg, "old password", locationUrl, 0);
		return;
	}

	//free result 
	mysqlFreeResult(&global_af_handler[rand_mh]);

	//set new password
	memset(sql_str, 0, sizeof(sql_str));
	sprintf(sql_str, "update af_member set passwd = '%s' where id = %d limit 1", enc_new_pwd, ucocfg.uid);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	if(!ret)
	{
		//query failed
		output_msgPage(r, facfg, ucocfg, "password update failed, try again", locationUrl, 0);
		return;
	}
	
	memset(locationUrl, 0, sizeof(locationUrl));
	sprintf(locationUrl, "%s/union/my", facfg->appRunUrl);
	output_msgPage(r, facfg, ucocfg, "password update success!", locationUrl, 1);
}


