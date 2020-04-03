/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_forum_manage.h
* file desc£ºforum topic mange
* author:AndyZhou<diudiu8848@163.com>
*/

#ifndef FORUM_MANAGE_H
#define FORUM_MANAGE_H

#ifdef __cplusplus
extern "C" {
#endif

//topic manage enter
void topic_manage(request_rec *r, af_app_config* facfg,
				  const char* board_bname, signal_board_info sbi,
				  apr_table_t* param_list);


#ifdef __cplusplus
}
#endif

#endif
