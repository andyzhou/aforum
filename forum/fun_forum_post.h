/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_forum_post.h
* file desc£ºforum post or reply topic
* author:AndyZhou<diudiu8848@163.com>
*/

#ifndef FORUM_POST_H
#define FORUM_POST_H

#ifdef __cplusplus
extern "C" {
#endif

//post topic form
void topic_post(request_rec *r, af_app_config* facfg,
				const char* board_bname, signal_board_info sbi,
				apr_table_t* param_list, int isEdit);

//save topic
void topic_save(request_rec *r, ApacheRequest* req, af_app_config* facfg, int isUpdate);


#ifdef __cplusplus
}
#endif

#endif
