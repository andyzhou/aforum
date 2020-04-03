/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_forum_board.c
* file desc£ºforum signal board entry
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

#include "fun_forum_public.h"
#include "fun_forum_topic.h"
#include "fun_forum_post.h"
#include "fun_forum_read.h"
#include "fun_forum_board.h"
#include "fun_forum_search.h"
#include "fun_forum_manage.h"


//signal board entery
void signal_board_entry(request_rec *r, af_app_config* facfg, apr_table_t* param_list)
{
	signal_board_info sbi = {0};//signal board info

	const char* board_bname = 0;//board bname
	const char* query_str = 0;
	const char* sub_cmd = 0;
	apr_table_t* sub_param_list;
	int ret = 0;


	//anlyse search query string
	query_str = ap_table_get(param_list, "1");
	signalDataGetData(r, query_str, &board_bname, &sub_param_list);

	sub_cmd = ap_table_get(sub_param_list, "cmd");

	//param check
	if(!board_bname || !strcmp(board_bname, "(null)"))
	{
		//show invalid page
		output_invalid_page(r, facfg);
		return;
	}

	//get signal board info
	ret = get_signal_board_info(r, (char*)board_bname, &sbi);

	if(!ret)
	{
		//show invalid page
		output_invalid_page(r, facfg);
		return;
	}

	if(sub_cmd && !strcmp(sub_cmd, "read"))
	{
		//read signal topic
		topic_read(r, facfg, board_bname, sbi, sub_param_list);
	}
	else if(sub_cmd && !strcmp(sub_cmd, "post"))
	{
		//post(reply) topic
		topic_post(r, facfg, board_bname, sbi, sub_param_list, 0);
	}
	else if(sub_cmd && !strcmp(sub_cmd, "edit"))
	{
		//edit topic
		topic_post(r, facfg, board_bname, sbi, sub_param_list, 1);
	}
	else if(sub_cmd && !strcmp(sub_cmd, "query"))
	{
		//query topic
		topic_search_result(r, facfg, board_bname, sbi, sub_param_list);
	}
	else if(sub_cmd && !strcmp(sub_cmd, "manage"))
	{
		//topic manage enter
		topic_manage(r, facfg, board_bname, sbi, sub_param_list);
	}
	else
	{
		//topics list
		list_topic(r, facfg, board_bname, sbi, sub_param_list);
	}

}

