/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_union_login.h
* file desc£ºmember login func define
* author:AndyZhou<diudiu8848@163.com>
*/

#ifndef _FUN_MEMBER_LOGIN_H
#define _FUN_MEMBER_LOGIN_H

//#include "fun_union_public.h"

void member_login(request_rec* r, af_app_config* uacfg, apr_table_t* param_list);


//login check
int member_login_check(request_rec* r, af_app_config* uacfg,
					   ApacheRequest* req);


//author by mysql
//1:ok 0:failed
int member_author_by_mysql(request_rec* r, af_app_config* uacfg,
						   		const char* uemail, const char* upwd,
						   		unionBasicInfo *ubi);

//author by ldap
int member_login_check_by_ldap(request_rec* r, af_app_config* uacfg,
							   		 ApacheRequest* req);

//exit
void member_exit(request_rec* r, af_app_config* uacfg);

#endif
