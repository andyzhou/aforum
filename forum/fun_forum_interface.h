/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name：fun_forum_interface.h
* file desc：forum module interface
* author:AndyZhou<diudiu8848@163.com>
*/


#ifndef FORUM_INTERFACE_H
#define FORUM_INTERFACE_H

//get type
int forum_get_method(request_rec* r, af_app_config* facfg, apr_table_t* param_list);

//post type
int forum_post_method(request_rec* r, af_app_config* facfg, ApacheRequest *req);

#endif

