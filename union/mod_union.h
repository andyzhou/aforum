/*
* Copyright (c) 2007
* All rights reserved.
* 
* 文件名称：mod_union.h
* 文件标识：统一会员模块函数定义
* 作    者：周志锐(丢丢)
*/

#ifndef MOD_UNION_H
#define MOD_UNION_H

//子进程初始化
static void union_child_init(server_rec *s, pool *p);

///模块初始化
void union_mod_init(request_rec* r);

//统一会员模块入口
static int union_handler(request_rec* r);

#endif
