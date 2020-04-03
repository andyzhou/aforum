/*
 * Copyright(c)2005
 * All rights reserved.
 *
 * file name:fun_forum_reg_user.h
 * description:registe new user from union
 *
 * author:zhirui
 * create date:2007-5-20
 * version:0.1
 */

#ifndef FC_FORUM_REG_USER_H
#define FC_FORUM_REG_USER_H

#ifdef __cplusplus
extern "C" {
#endif

//注册新会员到论坛用户表
void reg_user_from_union(request_rec *r, forum_app_config* facfg,
						 table* param_list);


#ifdef __cplusplus
}
#endif

#endif

