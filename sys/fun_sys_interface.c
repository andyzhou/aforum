/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_sys_interface.c
* file desc£ºsys module interface
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


#include "fun_sys_editboard.h"
#include "fun_sys_cats.h"
#include "fun_sys_interface.h"


//get type
int sys_get_method(request_rec* r, af_app_config* facfg, apr_table_t* param_list)
{
	char* cmd = 0;

	cmd = (char*)ap_table_get(param_list, "2");
	
	//chose operat
	if(cmd && !strcmp(cmd, "editboard"))
	{
		//show edit signal board
		sys_edit_board(r, facfg, param_list);
	}
	else if(cmd && !strcmp(cmd, "cats"))
	{
		//cats list
		sys_list_cat(r, facfg, param_list);
	}
	else if(cmd && !strcmp(cmd, "editcat"))
	{
		//edit signal cat
		sys_edit_cat(r, facfg, param_list);
	}
	else
	{
		//show_forum_home(r, facfg);
	}

	return OK;
}


//post type
int sys_post_method(request_rec* r, af_app_config* facfg, ApacheRequest *req)
{
	const char* cmd = 0;

	cmd = ApacheRequest_param(req, "cmd");

	if(cmd && !strcmp(cmd, "saveboard"))
	{
		//save or update signal board
		sys_save_board(r, facfg, req);
	}
	else if(cmd && !strcmp(cmd, "savecat"))
	{
		//save or update signal cat
		sys_save_cat(r, facfg, req);
	}

	return OK;
}

