/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_forum_search.h
* file desc£ºforum topic search funcs
* author:AndyZhou<diudiu8848@163.com>
*/

#ifndef FORUM_SEARCH_H
#define FORUM_SEARCH_H

//show search form
void topic_search_form(request_rec* r, af_app_config* facfg);

//show search result
void topic_search_result(request_rec* r, af_app_config* facfg,
					     const char* board_bname, signal_board_info sbi,
					     apr_table_t* param_list);

#endif




