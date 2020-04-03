/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_forum_read.h
* file desc£ºforum  read topic
* author:AndyZhou<diudiu8848@163.com>
*/

#ifndef FORUM_READ_H
#define FORUM_READ_H

#ifdef __cplusplus
extern "C" {
#endif

//read signal topic
void topic_read(request_rec *r, af_app_config* facfg,
				const char* board_bname, signal_board_info sbi,
				apr_table_t* param_list);


#ifdef __cplusplus
}
#endif

#endif

