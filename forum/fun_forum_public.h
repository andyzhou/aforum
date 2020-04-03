/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_forum_public.h
* file desc£ºforum public funcs
* author:AndyZhou<diudiu8848@163.com>
*/

#ifndef FORUM_PUBLIC_H
#define FORUM_PUBLIC_H

#include "fun_cfg.h"

//signal board simple info struct
typedef struct {
	char id[10];//board id
	char name[128];//board name
	char topics[10];//total topics
	char posts[10];//total posts
	char intro[1024];//board intro
	int  isOpen;//1:open 0:close
	int  browseLevel;
	int  postLevel;
} signal_board_info;


//signal root topic info struct
typedef struct {
	char title[128];//subject title
	int	 reply;//reply number
	int  locked;//topic locked
	int	 hoted;//topic is hoted
	int  toped;//topic is toped
} signal_root_topic_info;

//////////////////////////////////////


//get signal board simple info
//1:ok 0:failed
int get_signal_board_info(request_rec* r, char* board_bname, signal_board_info* sbi);


//get signal root topic info
//1:ok 0:failed
int get_signal_root_topic_info(request_rec* r, int board_id,
							   int topic_id, signal_root_topic_info* srti);


#endif
