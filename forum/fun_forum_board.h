/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_forum_board.h
* file desc£ºforum signal board entry
* author:AndyZhou<diudiu8848@163.com>
*/

#ifndef FORUM_BOARD_H
#define FORUM_BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

//signal board entry
void signal_board_entry(request_rec *r, af_app_config* facfg, apr_table_t* param_list);

#ifdef __cplusplus
}
#endif


#endif

