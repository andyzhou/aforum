/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_sys_cats.c
* file desc£ºsystem cat manage
* author£ºAndyZhou<diudiu8848@163.com>
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


#include "fun_sys_cats.h"

#define SYS_LIST_CAT_TPL "af_sys_cats.html"
#define SYS_EDIT_CAT_TPL "af_sys_editcat.html"


void sys_list_cat(request_rec* r, af_app_config* facfg, 
						  apr_table_t* param_list)
{
	tpl_t *my_tpl;
	char *content = 0;

	char sql_str[BIG_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	//show page header
	output_pageHeader(r, facfg, "Board Category",  ucocfg);

	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, SYS_TPL_DIR, SYS_LIST_CAT_TPL);

	//set general variable
	gen_pub_varible_set(r, facfg, &my_tpl);

	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	//show all cats list
	mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], "SET NAMES 'GB2312'", &errBuf);
	memset(sql_str, 0, sizeof(sql_str));
	snprintf(sql_str, sizeof(sql_str) - 1,  "select id, title, isOpen from af_boards where isOpen > 0 and pid = 0");
	
	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);
	
	if(ret)
	{
		rec_num = mysqlNumRows(&global_af_handler[rand_mh]);
		
		if(rec_num > 0)
		{
			//set sections list
			tpl_select_section(my_tpl, "catlists");

			while(global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]))
			{
				tpl_set_field_fmt(my_tpl, "cat_id", "%s", global_af_handler[rand_mh].pRow[0]);
				tpl_set_field_fmt(my_tpl, "cat_name", "%s", global_af_handler[rand_mh].pRow[1]);
				tpl_append_section(my_tpl);
			}

			tpl_deselect_section(my_tpl);

			//free result
			mysqlFreeResult(&global_af_handler[rand_mh]);
		}
	}

	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);

	//ouput page bottom
	output_pageBottom(r, facfg);
}


void sys_edit_cat(request_rec* r, af_app_config* facfg, 
						  apr_table_t* param_list)
{
	const char* catid = 0;
	tpl_t *my_tpl;
	char *content = 0;

	char sql_str[BIG_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	catid = (char*)ap_table_get(param_list, "3");

	if(catid && strcmp(catid, "(null)"))
	{
		//reload cat list
		sys_list_cat(r, facfg, param_list);
		return;
	}

	//show page header
	output_pageHeader(r, facfg, "create or edit category",  ucocfg);

	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, SYS_TPL_DIR, SYS_EDIT_CAT_TPL);

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


void sys_save_cat(request_rec* r, af_app_config* facfg, 
						  ApacheRequest* req)
{
	char sql_str[BIG_MEM_SIZE] = {0};
	char locationUrl[MEDIM_MEM_SIZE] = {0};
	char msgOk[SMALL_MEM_SIZE] = {0};
	char msgFail[SMALL_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	const char* cid = 0;
	const char* title = 0;
	const char* isOpen = 0;

	//get input param 
	cid		= ApacheRequest_param(req, "cid");
	title		= ApacheRequest_param(req, "title");
	isOpen	= ApacheRequest_param(req, "isOpen");

	sprintf(locationUrl, "%s/sys/cats", facfg->appRunUrl);

	if(!title || !(*title))
	{
		//error param
		output_msgPage(r, facfg, ucocfg, "need category name", locationUrl, 0);
		return;
	}

	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	//check new cat is empty
	if(!cid || atoi(cid) <= 0)
	{
		
		mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], "SET NAMES 'GB2312'", &errBuf);

		memset(sql_str, 0, sizeof(sql_str));
		sprintf(sql_str, "select title from af_boards where title = '%s' limit 1", title);

		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

		if(!ret)
		{
			//query failed
			output_msgPage(r, facfg, ucocfg, "query failed, try again", locationUrl, 0);
			return;
		}

		rec_num = mysqlNumRows(&global_af_handler[rand_mh]);

		if(rec_num > 0)
		{
			output_msgPage(r, facfg, ucocfg, "category exist", locationUrl, 0);
			return;			
		}

		//create new cat
		memset(sql_str, 0, sizeof(sql_str));
		sprintf(sql_str, "insert into af_boards(title, isOpen) values('%s', %d)", title, isOpen ? atoi(isOpen) : 0);
		sprintf(msgOk, "%s", "create category success");
		sprintf(msgFail, "%s", "create category failed");
	}
	else
	{
		//update old cat
		memset(sql_str, 0, sizeof(sql_str));
		sprintf(sql_str, "update af_boards set title = '%s', isOpen = %d where id = %d", title, isOpen ? atoi(isOpen) : 0, atoi(cid));
		sprintf(msgOk, "%s", "update category success");
		sprintf(msgFail, "%s", "update category failed");
	}

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	if(!ret)
	{
		//query failed
		output_msgPage(r, facfg, ucocfg, msgFail, locationUrl, 0);
		return;
	}

	
	output_msgPage(r, facfg, ucocfg, msgOk, locationUrl, 1);
	return;
}


