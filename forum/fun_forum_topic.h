/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_forum_topic.h
* file desc£ºforum signal board topic list
* author:AndyZhou<diudiu8848@163.com>
*/

#ifndef FORUM_TOPIC_LIST_H
#define FORUM_TOPIC_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

//topic list
void list_topic(request_rec *r, af_app_config* facfg,
				const char* board_bname, signal_board_info sbi,
				apr_table_t* param_list);

#ifdef __cplusplus
}
#endif

#endif
