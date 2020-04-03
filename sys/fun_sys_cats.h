/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_sys_cats.h
* file desc£ºsystem cat manage
* author£ºAndyZhou<diudiu8848@163.com>
*/

#ifndef SYS_EDITBOARD_H
#define SYS_EDITBOARD_H

void sys_list_cat(request_rec* r, af_app_config* facfg, apr_table_t* param_list);

void sys_edit_cat(request_rec* r, af_app_config* facfg, apr_table_t* param_list);

void sys_save_cat(request_rec* r, af_app_config* facfg, ApacheRequest* req);

#endif

