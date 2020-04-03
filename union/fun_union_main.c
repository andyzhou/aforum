/*
* Copyright (c) 2007
* All rights reserved.
* 
* fileName: fun_union_main.c
* description: union manage center
* author:zhirui
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

///#include "api_ldap.h"//use ldap for user author
#include "api_flate.h"
#include "api_util.h"

#include "fun_public.h"
#include "fun_union_errors.h"
#include "fun_union_main.h"


//show union manage center
void member_manage_main(request_rec* r, site_app_conf* scfg, cookie_info cocfg)
{
	Template *my_tpl;
	char* tpl_file = 0;
	char* url = 0;//return url

	//����û��Ƿ��¼
	if(!cocfg.available)
	{
		url = "/union";
		member_login_form(r, scfg, url);
		return;
	}

	//���ҳͷ
	output_union_pageHeader(r, scfg, userHeaderMsg[4], cocfg);

	//��ʼ��ģ������
	my_tpl = (Template *)ap_palloc(r->pool, sizeof(Template));
	my_tpl = (Template *)ap_pcalloc(r->pool, sizeof(Template));

	tpl_file = (char*)ap_palloc(r->pool, 256);
	tpl_file = (char*)ap_pcalloc(r->pool, 256);

	tpl_file = ap_psprintf(r->pool, "%s/union/member_main.html", scfg->tplPath);

	//load tpl file
	tplSetFile(r, &my_tpl, tpl_file);
	
	//����ҳ�����
	tplPrint(r, my_tpl);

	//���ҳβ
	//output_union_pageBottom(r);
}

