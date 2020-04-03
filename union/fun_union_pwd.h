/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name:fun_union_pwd.h
* file desc:member change password
* author:AndyZhou<diudiu8848@163.com>
*/

#ifndef UNION_PWD_H
#define UNION_PWD_H

//show edit pwd form
void member_edit_pwd(request_rec* r, af_app_config* facfg);

//save new pwd
void member_save_pwd(request_rec* r, af_app_config* facfg, ApacheRequest *req);

#endif
