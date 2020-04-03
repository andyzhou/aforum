/*
* Copyright (c) 2007
* All rights reserved.
* 
* 文件名称：fun_union_public.c
* 文件标识：会员公共函数
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
//#include "api_ldap.h"

#include "fun_cfg.h"
#include "fun_public.h"
#include "fun_union_errors.h"
#include "fun_union_public.h"



////////////////////////////////

//union generic page header
void output_union_pageHeader(request_rec* r, af_app_config *uacfg,
							 char* title, unionCookieInfo ucocfg)
{
	Template *my_tpl;
	char* tpl_file = 0;

	r->content_type = "text/html";
	ap_send_http_header(r);

	//初始化模板引擎
	my_tpl = (Template *)ap_palloc(r->pool, sizeof(Template));
	my_tpl = (Template *)ap_pcalloc(r->pool, sizeof(Template));

	tpl_file = (char*)ap_palloc(r->pool, 256);
	tpl_file = (char*)ap_pcalloc(r->pool, 256);

	if(ucocfg.available)
	{
		//user logined page header
		tpl_file = ap_psprintf(r->pool, "%s/union_page_lheader.html", uacfg->appTplPath);
	}
	else
	{
		tpl_file = ap_psprintf(r->pool, "%s/union_page_uheader.html", uacfg->appTplPath);
	}	

	//载入模板文件
	tplSetFile(r, &my_tpl, tpl_file);
	tplSetVar(my_tpl, "site_title", uacfg->appName);
	tplSetVar(my_tpl, "site_url", uacfg->appUrl);
	tplSetVar(my_tpl, "item_name", title ? title : "");
	tplSetVar(my_tpl, "user_nickname", ucocfg.nickname);

	//最终页面输出
	tplPrint(r, my_tpl);
}


//union page bottom
void output_union_pageBottom(request_rec* r, af_app_config *uacfg)
{
	Template *my_tpl;
	char* tpl_file = 0;

	//初始化模板引擎
	my_tpl = (Template *)ap_palloc(r->pool, sizeof(Template));
	my_tpl = (Template *)ap_pcalloc(r->pool, sizeof(Template));

	tpl_file = (char*)ap_palloc(r->pool, 256);
	tpl_file = (char*)ap_pcalloc(r->pool, 256);

	tpl_file = ap_psprintf(r->pool, "%s/union_page_bottom.html", uacfg->appTplPath);

	//载入模板文件
	tplSetFile(r, &my_tpl, tpl_file);

	//最终页面输出
	tplPrint(r, my_tpl);
}


//show union error msg page
void show_union_err_msg(request_rec* r, af_app_config *uacfg, 
						char* errMsg, char* returnUrl, int ok_or_failed)
{
	Template *my_tpl;
	char* tpl_file = 0;
	char* url = 0;//return url

	char tmp_tpl_file[64] = {0};
	char page_title[32] = {0};

	switch(ok_or_failed)
	{

	case 1://failed
		strcpy(page_title, userHeaderMsg[0]);
		strcpy(tmp_tpl_file, "union_msg");
		break;

	case 2://success
		strcpy(page_title, userHeaderMsg[1]);
		strcpy(tmp_tpl_file, "union_msg_ok");
		break;

	default:
		strcpy(page_title, userHeaderMsg[0]);
		strcpy(tmp_tpl_file, "union_msg");
		break;
	}

	//输出页头
	output_union_pageHeader(r, uacfg, userErrMsg[0], ucocfg);

	//初始化模板引擎
	my_tpl = (Template *)ap_palloc(r->pool, sizeof(Template));
	my_tpl = (Template *)ap_pcalloc(r->pool, sizeof(Template));

	tpl_file = (char*)ap_palloc(r->pool, 256);
	tpl_file = (char*)ap_pcalloc(r->pool, 256);

	tpl_file = ap_psprintf(r->pool, "%s/%s.html", uacfg->appTplPath, tmp_tpl_file);

	//load tpl file
	tplSetFile(r, &my_tpl, tpl_file);

	//set var variable
	tplSetVar(my_tpl, "sysMsg", errMsg ? errMsg : userErrMsg[0]);
	tplSetVar(my_tpl, "returnUrl", returnUrl ? returnUrl : "/union");
	
	//最终页面输出
	tplPrint(r, my_tpl);

	//输出页尾
	output_union_pageBottom(r, uacfg);
}



/*
//get signal user node
//ret -1:error param  0:failed 1:ok
int union_get_siguser(request_rec* r, site_app_conf* scfg,
					  char* uemail, sigNodeVal **snv)
{
	char searchDnStr[128] = {0};
	char filterStr[64] = {0};
	int ret = 0;

	if(!uemail || !(*uemail))
	{
		return -1;
	}

	snprintf(searchDnStr, sizeof(searchDnStr) - 1, "cn=%s,%s", SEARCH_DN_NAME, scfg->ldapRootDn);
	snprintf(filterStr, sizeof(filterStr) - 1, "(uid=%s)", uemail);

	//get signal node
	ret = ldapGetSignalNode(ldapOpt.ld, searchDnStr, filterStr, snv);

	if(ret > 0)
	{
		//email has suit, return suit node attribute number
		return ret;
	}
	else
	{
		//email not suit
		return 0;
	}
}
*/



