/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_forum_interface.c
* file desc£ºforum module interface
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


#include "fun_forum_index.h"
#include "fun_forum_board.h"
#include "fun_forum_interface.h"


//get type
int forum_get_method(request_rec* r, af_app_config* facfg, apr_table_t* param_list)
{
	char* subboard = 0;

	getSpecGetData(r, &param_list);
	subboard = (char*)ap_table_get(param_list, "1");

	//check cmd is number
	//ap_set_content_type(r, "text/html");
	//ap_rprintf(r, "<h1>subboard:%s</h1>", subboard);
	
	//chose operat
	if(subboard && strcmp(subboard, "(null)") && strlen(subboard) > 0)
	{
		//show signal board entry
		signal_board_entry(r, facfg, param_list);
	}
	else
	{
		show_forum_home(r, facfg);
	}

	return OK;
}


//post type
int forum_post_method(request_rec* r, af_app_config* facfg, ApacheRequest *req)
{
	const char* cmd = 0;

	cmd =  ApacheRequest_param(req, "cmd");

	if(cmd && !strcmp(cmd, "savetopic"))
	{
		//save topic
		topic_save(r, req, facfg, 0);
	}
	else if(cmd && !strcmp(cmd, "uptopic"))
	{
		//save topic
		topic_save(r, req, facfg, 1);
	}

	return OK;
}

