/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name：fun_union_interface.c
* file desc：member module interface
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



#include "fun_cfg.h"
#include "tpllib.h"
#include "fun_public.h"

#include "apache_request.h"


#include "fun_union_login.h"
#include "fun_union_reg.h"
#include "fun_union_my.h"

#include "fun_union_profile.h"
#include "fun_union_interface.h"


//get type
int union_get_method(request_rec* r, af_app_config* uacfg, apr_table_t* param_list)
{
	int param_num = 0;
	int ret = 0;
	char* cmd = 0;//operate command

	apr_table_t* sub_param_list;

	//cmd of GET method
	cmd = (char*)ap_table_get(param_list, "2");

	//ap_set_content_type(r, "text/html");
	//ap_rprintf(r, "<h1>cmd:%s</h1>", cmd);

	//chose operat	
	if(cmd && !strcmp(cmd, "login"))
	{
		//show member login page
		member_login(r, uacfg, param_list);
	}
	else if(cmd && !strcmp(cmd, "reg"))
	{
		//show new member reg page
		member_reg(r, uacfg);
	}
	else if(cmd && !strcmp(cmd, "checkNewUser"))
	{
		//new memeber valid check
		member_check_valid(r, uacfg, param_list);
	}
	else if(cmd && !strcmp(cmd, "exit"))
	{
		//member exit
		member_exit(r, uacfg);
	}
	else if(cmd && !strcmp(cmd, "my"))
	{
		//show member manage center
		member_manage_main(r, uacfg, param_list);
	}
	else
	{
		member_manage_main(r, uacfg, param_list);
	}

}


//post type
int union_post_method(request_rec* r, af_app_config* uacfg, ApacheRequest *req)
{
	const char* cmd = 0;

	/*
	r->content_type = "text/html";
	ap_send_http_header(r);
	ap_rprintf(r, "<h1>cmd:%s</h1>", "xx");	
	return OK;
	*/

	cmd = ApacheRequest_param(req, "cmd");

	if(cmd && !strcmp(cmd, "save"))
	{
		//new registe save
		member_reg_save(r, uacfg, req);
	}
	else if(cmd && !strcmp(cmd, "loginCheck"))
	{
		//user login check
		member_login_check(r, uacfg, req);
	}
	else if(cmd && !strcmp(cmd, "saveProf"))
	{
		//save user profile
		member_profile_save(r, uacfg, req);
	}
	else if(cmd && !strcmp(cmd, "savePwd"))
	{
		//save user new password
		member_save_pwd(r, uacfg, req);
	}
	else
	{
		//show member login page
		member_login(r, uacfg, NULL);
	}
}

