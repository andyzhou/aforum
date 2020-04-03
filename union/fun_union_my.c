/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_union_my.c
* file desc£ºmember manage center
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

#include "fun_union_profile.h"
#include "fun_union_pwd.h"
#include "fun_union_my.h"

#define UNION_MY_TPL "af_my.html"

//show union manage center
void member_manage_main(request_rec* r, af_app_config *facfg, apr_table_t* param_list)
{
	tpl_t *my_tpl;
	char* content = 0;
	char location_url[MEDIM_MEM_SIZE] = {0};
	const char* subcmd = 0;

	//check current user is logined
	if(!ucocfg.available)
	{
		//internal redirect
		sprintf(location_url, "/%s/union/login/:union:my", facfg->appRunUrl);
		ap_internal_redirect(location_url, r);
		return;
	}

	subcmd = ap_table_get(param_list, "3");

	//sub function relocation
	if(subcmd && !strcmp(subcmd, "edit"))
	{
		//edit profifle
		member_profile_edit(r, facfg);
		return;
	}
	else if(subcmd && !strcmp(subcmd, "editpwd"))
	{
		//change password
		member_edit_pwd(r, facfg);
		return;
	}

	//show page header
	output_pageHeader(r, facfg, "Manage Center",  ucocfg);


	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, UNION_TPL_DIR, UNION_MY_TPL);

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

