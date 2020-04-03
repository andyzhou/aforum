/*
* Copyright (c) 2006
* All rights reserved.
* 
* file: fun_member_exit.c
* file desc£ºmember exit func
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
#include "ap_alloc.h"

#include "apache_request.h"
#include "apache_cookie.h"

#include "api_flate.h"
#include "api_util.h"

#include "fun_cfg.h"
#include "fun_public.h"
#include "fun_union_errors.h"
#include "fun_union_public.h"
#include "fun_union_exit.h"


void member_exit(request_rec* r, af_app_config* uacfg)
{
	const char* location_url = "/union";

	ApacheCookie *apc_uid = ApacheCookie_new(r, 
					     "-name", union_uid_cookie_name, 
					     "-value", NULL,
                         "-expires", "+1M",
					     NULL);
	
	ApacheCookie *apc_ename = ApacheCookie_new(r, 
					     "-name", union_uename_cookie_name, 
					     "-value", NULL,
                         "-expires", "+1M",
					     NULL);


	ApacheCookie *apc_nick = ApacheCookie_new(r, 
					     "-name", union_unick_cookie_name, 
					     "-value", NULL,
                         "-expires", "+1M",
					     NULL);


	ApacheCookie *apc_uhash = ApacheCookie_new(r, 
					     "-name", union_uhash_cookie_name, 
					     "-value", NULL,
                         "-expires", "+1M",
					     NULL);
	
	ApacheCookie_bake(apc_uid);
	ApacheCookie_bake(apc_ename);
	ApacheCookie_bake(apc_nick);
	ApacheCookie_bake(apc_uhash);

	ap_internal_redirect(location_url, r);
}

