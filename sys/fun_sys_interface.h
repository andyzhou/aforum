/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_sys_interface.h
* file desc£ºsys module interface
* author:AndyZhou<diudiu8848@163.com>
*/


#ifndef SYS_INTERFACE_H
#define SYS_INTERFACE_H

//get type
int sys_get_method(request_rec* r, af_app_config* facfg, apr_table_t* param_list);

//post type
int sys_post_method(request_rec* r, af_app_config* facfg, ApacheRequest *req);

#endif

