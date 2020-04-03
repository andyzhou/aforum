/*
* Copyright (c) 2006
* All rights reserved.
* 
* �ļ����ƣ�fun_member_getpwd.c
* �ļ���ʶ����Ա��ȡ���뺯��
* ��    �ߣ���־��(����)
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

	//���ҳͷ
	output_page_header_js(r, scfg, "�ػ�����", cocfg);

	//��ʼ��ģ������
	my_tpl = (Template *)ap_palloc(r->pool, sizeof(Template));
	my_tpl = (Template *)ap_pcalloc(r->pool, sizeof(Template));

	tpl_file = (char*)ap_palloc(r->pool, 256);
	tpl_file = (char*)ap_pcalloc(r->pool, 256);

	tpl_file = ap_psprintf(r->pool, "%s/reglogin/member_getpwd.html", scfg->tplPath);

	//����ģ���ļ�
	tplSetFile(r, &my_tpl, tpl_file);

	tplSetVar(my_tpl, "return_url", r->uri);
	
	//����ҳ�����
	tplPrint(r, my_tpl);

	//���ҳβ
	//output_page_bottom_js(r, scfg);
}

