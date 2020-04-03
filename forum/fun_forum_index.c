/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_forum_index.c
* file desc£ºforum home
* author:AndyZhou<diudiu8848@163.com>
*/

#include <stdio.h>
#include <string.h>

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_main.h"
#include "http_log.h"
#include "http_protocol.h"

#include "apr_buckets.h"
#include "apr_strings.h"
#include "apr_general.h"
#include "apr_lib.h"
#include "apr_compat.h"
#include "apr_pools.h"


#include "apache_request.h"
#include "fun_cfg.h"
#include "tpllib.h"
#include "fun_public.h"

#include "fun_forum_index.h"


//define tpl file
#define FORUM_HOME_TPL "af_index.html"
#define FORUM_HOME_SUB_BOARD_TPL "af_sub_boards.html"
#define FORUM_SYS_MENU_TPL "af_sys_menu.html"


typedef struct catBoardInfo {
	int  cat_board_id;//board id
	int  parent_cat_id;//parent board id
	int  subject_num;//topics
	int  post_num;//posts
	char title[MEDIM_MEM_SIZE];//title
	char bname[SMALL_MEM_SIZE];//mark
	char intro[BIG_MEM_SIZE];//intro
	char moderator[MEDIM_MEM_SIZE];//board moderator
	char last_post[MEDIM_MEM_SIZE];//last post time and user
	int  last_update;//
	int  show_type;//board show type 2:tree 1:thread
	int	 is_locked;//board is set password
} catBoardInfo;


//get bords info of signal cat
char *get_sub_board_list(request_rec *r, af_app_config* facfg, int cat_id,
						 struct catBoardInfo *suit_cat_board, int data_num);

//show sys manage menu page
void show_sys_manage_page(request_rec *r, af_app_config* facfg);

//forum home page
void show_forum_home(request_rec *r, af_app_config* facfg)
{
	tpl_t *my_tpl;
	struct catBoardInfo *suit_cat_board;

	char sql_str[MEDIM_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	const char *result = NULL;
	const char *tmp_result = NULL;
	char* content = 0;

	int i = 0;
	int m = 0;
	int n = 0;
	int total_subject_num = 0;
	int total_post_num = 0;

	char total_subject_str[TINY_MEM_SIZE] = {0};
	char total_post_str[TINY_MEM_SIZE] = {0};

	//show page header
	output_pageHeader(r, facfg, "Home",  ucocfg);

	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, FORUM_TPL_DIR, FORUM_HOME_TPL);

	//set general variable
	gen_pub_varible_set(r, facfg, &my_tpl);

	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	//get all cats and boards	
	//mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], "SET NAMES 'GB2312'", &errBuf);

	if(!global_af_handler[rand_mh].pConn)
	{
		mysqlSimpConnect(r->pool, af_db_cfg, &global_af_handler[rand_mh], &errBuf);
	}

	memset(sql_str, 0, sizeof(sql_str));
	snprintf(sql_str, sizeof(sql_str) - 1,  "select id, title, bname, pid, topics, posts, intro, lastUpdate, \
				viewType, lastPost, moderator, passwd from af_boards where isOpen = 1 order by fcat, sortNum");
	
	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);
	
	if(ret)
	{
		rec_num = mysqlNumRows(&global_af_handler[rand_mh]);
		
		if(rec_num > 0)
		{
			
			//define dynamic struct array
			suit_cat_board = (struct catBoardInfo *)apr_palloc(r->pool, rec_num * sizeof(struct catBoardInfo));
			suit_cat_board = (struct catBoardInfo *)apr_pcalloc(r->pool, rec_num * sizeof(struct catBoardInfo));

			while(global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]))
			{
				suit_cat_board[i].cat_board_id = global_af_handler[rand_mh].pRow[0] ? atoi(global_af_handler[rand_mh].pRow[0]) : 0;//board id
				sprintf(suit_cat_board[i].title, "%s", global_af_handler[rand_mh].pRow[1] ? global_af_handler[rand_mh].pRow[1] : "");//title
				sprintf(suit_cat_board[i].bname, "%s", global_af_handler[rand_mh].pRow[2] ? global_af_handler[rand_mh].pRow[2] : "");//bname
				
				suit_cat_board[i].parent_cat_id = global_af_handler[rand_mh].pRow[3] ? atoi(global_af_handler[rand_mh].pRow[3]) : 0;//pid
				suit_cat_board[i].subject_num = global_af_handler[rand_mh].pRow[4] ? atoi(global_af_handler[rand_mh].pRow[4]) : 0;//topics
				suit_cat_board[i].post_num = global_af_handler[rand_mh].pRow[5] ? atoi(global_af_handler[rand_mh].pRow[5]) : 0;//posts
				sprintf(suit_cat_board[i].intro, "%s", global_af_handler[rand_mh].pRow[6] ? global_af_handler[rand_mh].pRow[6] : "");//intro
				suit_cat_board[i].last_update = global_af_handler[rand_mh].pRow[7] ? atoi(global_af_handler[rand_mh].pRow[7]) : 0;
				suit_cat_board[i].show_type = global_af_handler[rand_mh].pRow[8] ? atoi(global_af_handler[rand_mh].pRow[8]) : 1;

				snprintf(suit_cat_board[i].last_post, sizeof(suit_cat_board[i].last_post) - 1, "%s",
						 	global_af_handler[rand_mh].pRow[9] ? global_af_handler[rand_mh].pRow[9] : "");//last post time and user

				//moderator
				snprintf(suit_cat_board[i].moderator, sizeof(suit_cat_board[i].moderator) - 1, "%s", 
							global_af_handler[rand_mh].pRow[10] ? global_af_handler[rand_mh].pRow[10] : "");

				//set passwd
				if(strlen(global_af_handler[rand_mh].pRow[10]) > 0)
				{
					suit_cat_board[i].is_locked = 1;
				}
				else
				{
					suit_cat_board[i].is_locked = 0;
				}

				if(suit_cat_board[i].parent_cat_id != 0)
				{
					total_subject_num += suit_cat_board[i].subject_num;
					total_post_num += suit_cat_board[i].post_num;
				}

				i++;
			}

			//free result
			mysqlFreeResult(&global_af_handler[rand_mh]);
		}
	}


	//set sections list
	tpl_select_section(my_tpl, "catboardlist");

	for(m = 0; m < rec_num; m++)
	{
		//list cats
		if(suit_cat_board[m].parent_cat_id == 0)
		{
			//list suit sub boards
			tmp_result = get_sub_board_list(r, facfg, suit_cat_board[m].cat_board_id, suit_cat_board, rec_num);
			tpl_set_field_fmt(my_tpl, "show_cat_board_list", "%s", tmp_result);
			tpl_append_section(my_tpl);
		}
	}

	tpl_deselect_section(my_tpl);

	sprintf(total_subject_str, "%d", total_subject_num);
	sprintf(total_post_str, "%d", total_post_num);

	tpl_set_field_fmt_global(my_tpl, "show_topics_num", "%s", total_subject_str);	
	tpl_set_field_fmt_global(my_tpl, "show_posts_num", "%s", total_post_str);
	
	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);
	
	//load sys manage menu
	if(ucocfg.type == 4)
	{
		show_sys_manage_page(r, facfg);
	}

	//ouput page bottom
	output_pageBottom(r, facfg);
}


//get sub boards of signal cat
char *get_sub_board_list(request_rec *r, af_app_config* facfg, int cat_id,
						 struct catBoardInfo *suit_cat_board, int data_num)
{
	tpl_t *_my_tpl;
	char* content = 0;

	const char *return_tpl_content = NULL;
	char manage_url[SMALL_MEM_SIZE] = {0};

	char lock_img[SMALL_MEM_SIZE] = {0};
	char tmp_str[TINY_MEM_SIZE] = {0};
	char tmp_str_1[TINY_MEM_SIZE] = {0};
	char tmp_str_2[TINY_MEM_SIZE] = {0};
	char tmp_str_3[TINY_MEM_SIZE] = {0};
	int i = 0;

	struct tm *now_time;
	time_t nowtime;
	char cur_day[TINY_MEM_SIZE] = {0};
	const char* tmp_img = 0;
	const char* show_type = 0;

	//get current day value
	nowtime = time(NULL);
	now_time = localtime(&nowtime);
	strftime(cur_day, TINY_MEM_SIZE, "%y%m%d", now_time);

	//tpl file init
	tpl_file_init(r, facfg, &_my_tpl, FORUM_TPL_DIR, FORUM_HOME_SUB_BOARD_TPL);

	//set general variable
	gen_pub_varible_set(r, facfg, &_my_tpl);

	for(i =0; i < data_num; i++)
	{
		//signal cat name
		if(suit_cat_board[i].cat_board_id == cat_id)
		{
			//ap_rprintf(r, "<h1>a:%d, b:%d, title:%s</h1>", suit_cat_board[i].cat_board_id, cat_id, suit_cat_board[i].title);
			tpl_set_field_fmt_global(_my_tpl, "show_cat_name", "%s", suit_cat_board[i].title);
			break;
		}
	}

	//set sections list
	tpl_select_section(_my_tpl, "boardlist");

	for(i =0; i < data_num; i++)
	{                                                                                                                                                                                                                                                                                                                                                                                                                              
		if(suit_cat_board[i].parent_cat_id == cat_id)
		{	
			memset(tmp_str, 0, sizeof(tmp_str));
			memset(tmp_str_1, 0, sizeof(tmp_str_1));
			memset(tmp_str_2, 0, sizeof(tmp_str_2));
			memset(tmp_str_3, 0, sizeof(tmp_str_3));

			sprintf(tmp_str, "%d", suit_cat_board[i].cat_board_id);
			sprintf(tmp_str_1, "%d", cat_id);
			sprintf(tmp_str_2, "%d", suit_cat_board[i].subject_num);
			sprintf(tmp_str_3, "%d", suit_cat_board[i].post_num);


			tpl_set_field_fmt(_my_tpl, "show_cat_id", "%s", tmp_str_1);
			tpl_set_field_fmt(_my_tpl, "show_board_id", "%s", tmp_str);
			tpl_set_field_fmt(_my_tpl, "show_board_bname", "%s", suit_cat_board[i].bname ? suit_cat_board[i].bname : tmp_str);

			tpl_set_field_fmt(_my_tpl, "show_board_name", "%s", suit_cat_board[i].title);
			tpl_set_field_fmt(_my_tpl, "show_board_intro", "%s", suit_cat_board[i].intro);
			tpl_set_field_fmt(_my_tpl, "show_subject_num", "%s", tmp_str_2);
			tpl_set_field_fmt(_my_tpl, "show_post_num", "%s", tmp_str_3);
			tpl_set_field_fmt(_my_tpl, "show_last_post", "%s", suit_cat_board[i].last_post);
			tpl_set_field_fmt(_my_tpl, "show_moderator", "%s", suit_cat_board[i].moderator);

			//board stat img
			if(suit_cat_board[i].last_update != atoi(cur_day))
			{
				tmp_img = "board.gif";
			}
			else
			{
				tmp_img = "board_n.gif";
			}
			
			tpl_set_field_fmt(_my_tpl, "on_off_board_img", "%s", tmp_img);
			
			//show type
			switch(suit_cat_board[i].show_type)
			{
			case 2:
				show_type = "s=2";
				break;

			default:
				show_type = "s=1";
				break;
			}

			tpl_set_field_fmt(_my_tpl, "show_type", "%s", show_type);

			//if sys manager, show edit link
			memset(manage_url, 0, sizeof(manage_url));
			if(ucocfg.type == 4)
			{
				snprintf(manage_url, sizeof(manage_url) - 1, "<a href=%s/sys/editboard/%s>Edit</a>", facfg->appRunUrl, suit_cat_board[i].bname);
			}
			tpl_set_field_fmt(_my_tpl, "sys_board_edit", "%s", manage_url);

			//is locked
			memset(lock_img, 0, sizeof(lock_img));

			if(suit_cat_board[i].is_locked == 1)
			{
				snprintf(lock_img, sizeof(lock_img) - 1, "<img src=%s/images/lock.gif alt='Lock'>", facfg->appUrl);
			}
			tpl_set_field_fmt(_my_tpl, "show_lock_img", "%s", lock_img);

			
			tpl_append_section(_my_tpl);
		}
	}

	tpl_deselect_section(_my_tpl);

	//get tpl content
	content = apr_palloc(r->pool, tpl_length(_my_tpl) + 1);
	tpl_get_content(_my_tpl, content);

	tpl_free(_my_tpl);

	return content;
}


//show sys manage menu page
void show_sys_manage_page(request_rec *r, af_app_config* facfg)
{
	tpl_t *my_tpl;
	char *content = 0;

	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, SYS_TPL_DIR, FORUM_SYS_MENU_TPL);

	//set global tpl variable
	gen_pub_varible_set(r, facfg, &my_tpl);

	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);
}


