/*
* Copyright (c) 2006
* All rights reserved.
* 
* 文件名称：fun_member_getpwd.h
* 文件标识：会员获取密码函数定义
* 作    者：周志锐(丢丢)
*/

#ifndef _FUN_MEMBER_GETPWD_H
#define _FUN_MEMBER_GETPWD_H

void member_getpwd(request_rec* r, site_app_conf* scfg);


void member_send_pwd(request_rec* r, site_app_conf* scfg,
					 db_cfg cfg, ApacheRequest* req);


#endif
