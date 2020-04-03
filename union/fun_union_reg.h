/*
* Copyright (c) 2006
* All rights reserved.
* 
* file name£ºfun_union_reg.h
* file desc£ºmember registe funcs
* author:AndyZhou<diudiu8848@163.com>
*/

#ifndef FUN_UNION_REG_H
#define FUN_UNION_REG_H

//user reg form
void member_reg(request_rec* r, af_app_config* uacfg);

//new user check
int member_check_new(request_rec *r, char* uname, af_app_config* uacfg);

//check user is registed
void member_check_valid(request_rec* r, af_app_config* uacfg, apr_table_t* param_list);

//user reg save
void member_reg_save(request_rec* r, af_app_config* uacfg, ApacheRequest* req);

//new user save by ldap mode
void _member_reg_save_by_ldap(request_rec* r, af_app_config* uacfg);

#endif
