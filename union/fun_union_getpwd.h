/*
* Copyright (c) 2006
* All rights reserved.
* 
* �ļ����ƣ�fun_member_getpwd.h
* �ļ���ʶ����Ա��ȡ���뺯������
* ��    �ߣ���־��(����)
*/

#ifndef _FUN_MEMBER_GETPWD_H
#define _FUN_MEMBER_GETPWD_H

void member_getpwd(request_rec* r, site_app_conf* scfg);


void member_send_pwd(request_rec* r, site_app_conf* scfg,
					 db_cfg cfg, ApacheRequest* req);


#endif
