/*
* Copyright (c) 2006
* All rights reserved.
* 
* 文件名称：fun_member_getpwd.c
* 文件标识：会员获取密码函数
* 作    者：周志锐(丢丢)
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

#include "api_mysql.h"
#include "api_flate.h"
#include "api_util.h"

#include "fun_cfg.h"
#include "fun_public.h"
#include "fun_member_getpwd.h"

void member_getpwd(request_rec* r, site_app_conf* scfg)
{
	Template *my_tpl;
	char* tpl_file = 0;

	//输出页头
	output_page_header_js(r, scfg, "重获密码", cocfg);

	//初始化模板引擎
	my_tpl = (Template *)ap_palloc(r->pool, sizeof(Template));
	my_tpl = (Template *)ap_pcalloc(r->pool, sizeof(Template));

	tpl_file = (char*)ap_palloc(r->pool, 256);
	tpl_file = (char*)ap_pcalloc(r->pool, 256);

	tpl_file = ap_psprintf(r->pool, "%s/reglogin/member_getpwd.html", scfg->tplPath);

	//载入模板文件
	tplSetFile(r, &my_tpl, tpl_file);

	tplSetVar(my_tpl, "return_url", r->uri);
	
	//最终页面输出
	tplPrint(r, my_tpl);

	//输出页尾
	//output_page_bottom_js(r, scfg);
}

