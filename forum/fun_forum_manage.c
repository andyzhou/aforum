/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_forum_manage.c
* file desc£ºforum topic mange
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

#include "fun_forum_public.h"
#include "fun_forum_manage.h"


//delete signal topic and sub topics
void _topic_delete_all(request_rec *r, af_app_config* facfg, const char* board_bname,
					   signal_board_info sbi, apr_table_t* param_list);


//top or untop topic
void _topic_top_untop(request_rec *r, af_app_config* facfg, const char* board_bname,
					  signal_board_info sbi, apr_table_t* param_list, int top_or_untop);


//hot or unhot topic
void _topic_hot_unhot(request_rec *r, af_app_config* facfg, const char* board_bname,
					  signal_board_info sbi, apr_table_t* param_list, int hot_or_unhot);


//lock or unlock topic
void _topic_lock_unlock(request_rec *r, af_app_config* facfg, const char* board_bname,
					    signal_board_info sbi, apr_table_t* param_list, int lock_or_unlock);

//topic manage enter
void topic_manage(request_rec *r, af_app_config* facfg,
				  const char* board_bname, signal_board_info sbi,
				  apr_table_t* param_list)
{
	const char* sub_cmd = 0;

	//check operater is suited
	if(ucocfg.type != 4)
	{
		return;
	}

	sub_cmd = ap_table_get(param_list, "subcmd");

	if(sub_cmd && !strcmp(sub_cmd, "del"))
	{
		_topic_delete_all(r, facfg, board_bname, sbi, param_list);
	}
	else if(sub_cmd && !strcmp(sub_cmd, "uhot"))
	{
		_topic_hot_unhot(r, facfg, board_bname, sbi, param_list, 0);
	}
	else if(sub_cmd && !strcmp(sub_cmd, "hot"))
	{
		_topic_hot_unhot(r, facfg, board_bname, sbi, param_list, 1);
	}
	else if(sub_cmd && !strcmp(sub_cmd, "ulock"))
	{
		_topic_lock_unlock(r, facfg, board_bname, sbi, param_list, 0);
	}
	else if(sub_cmd && !strcmp(sub_cmd, "lock"))
	{
		_topic_lock_unlock(r, facfg, board_bname, sbi, param_list, 1);
	}
	else if(sub_cmd && !strcmp(sub_cmd, "utop"))
	{
		_topic_top_untop(r, facfg, board_bname, sbi, param_list, 0);
	}
	else if(sub_cmd && !strcmp(sub_cmd, "top"))
	{
		_topic_top_untop(r, facfg, board_bname, sbi, param_list, 1);
	}
}



////////// sub functions

//delete signal topic and sub topics
void _topic_delete_all(request_rec *r, af_app_config* facfg, const char* board_bname,
					   signal_board_info sbi, apr_table_t* param_list)
{
	char sql_str[MEDIM_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	char tmp_str[SMALL_MEM_SIZE] = {0};
	const char* total_posts = 0;
	const char* root_topic_id = 0;
	int int_root_topic_id = 0;

	root_topic_id = ap_table_get(param_list, "t");
	int_root_topic_id = root_topic_id ? atoi(root_topic_id) : 0;

	if(int_root_topic_id <= 0)
	{
		//show invalid page
		output_invalid_page(r, facfg);
		return;
	}

	//relocation url
	snprintf(tmp_str, sizeof(tmp_str) - 1, "%s/%s", facfg->appRunUrl, board_bname);

	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	if(!global_af_handler[rand_mh].pConn)
	{
		mysqlSimpConnect(r->pool, af_db_cfg, &global_af_handler[rand_mh], &errBuf);
	}


	//get topic and posts number
	memset(sql_str, 0, sizeof(sql_str));
	snprintf(sql_str, sizeof(sql_str) - 1, "select count(*) from af_topic_%s where rid = %d",
				sbi.id, int_root_topic_id);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	if(!ret)
	{
		//read topic faild
		output_msgPage(r, facfg, ucocfg, "read topic status failed.", tmp_str, 0);
		return;
	}

	rec_num = mysqlNumRows(&global_af_handler[rand_mh]);

	global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]);
	total_posts = global_af_handler[rand_mh].pRow[0];

	if(!total_posts || atoi(total_posts) < 1)
	{
		//read topic faild
		output_msgPage(r, facfg, ucocfg, "Not suit topic for delete.", tmp_str, 0);
		return;
	}


	//delete topic content
	memset(sql_str, 0, sizeof(sql_str));
	snprintf(sql_str, sizeof(sql_str) - 1, "delete from af_content_%s where rid = %d",
				sbi.id, int_root_topic_id);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	if(!ret)
	{
		//read topic faild
		output_msgPage(r, facfg, ucocfg, "delete topic content failed.", tmp_str, 0);
		return;
	}


	//delete topic and sub topics
	memset(sql_str, 0, sizeof(sql_str));
	snprintf(sql_str, sizeof(sql_str) - 1, "delete from af_topic_%s where rid = %d",
				sbi.id, int_root_topic_id);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	if(!ret)
	{
		//read topic faild
		output_msgPage(r, facfg, ucocfg, "delete topic failed.", tmp_str, 0);
		return;
	}


	//update topics and posts of signal board
	memset(sql_str, 0, sizeof(sql_str));
	snprintf(sql_str, sizeof(sql_str) - 1, "update af_boards set topics = topics - 1, posts = posts - %d \
											where id = %d", atoi(total_posts), atoi(sbi.id));

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);


	output_msgPage(r, facfg, ucocfg, "delete topic success", tmp_str, 1);

}



//top or untop topic
//top_or_untop 0:untop 1:top
void _topic_top_untop(request_rec *r, af_app_config* facfg, const char* board_bname,
					  signal_board_info sbi, apr_table_t* param_list, int top_or_untop)
{
	char sql_str[MEDIM_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	char tmp_str[MEDIM_MEM_SIZE] = {0};
	char desc_str[MEDIM_MEM_SIZE] = {0};
	char top_str[SMALL_MEM_SIZE] = {0};
	const char* total_posts = 0;
	const char* root_topic_id = 0;
	int int_root_topic_id = 0;

	root_topic_id = ap_table_get(param_list, "t");
	int_root_topic_id = root_topic_id ? atoi(root_topic_id) : 0;

	if(int_root_topic_id <= 0)
	{
		//show invalid page
		output_invalid_page(r, facfg);
		return;
	}

	//relocation url
	snprintf(tmp_str, sizeof(tmp_str) - 1, "%s/%s?cmd=read&t=%d", facfg->appRunUrl, board_bname, int_root_topic_id);

	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	memset(desc_str, 0, sizeof(desc_str));
	memset(sql_str, 0, sizeof(sql_str));

	if(top_or_untop)
	{
		sprintf(desc_str, "set top success");
		sprintf(top_str, "%ld", TOP_TOPIC_TIME_VALUE);
	}
	else
	{
		sprintf(desc_str, "cancel top success");
		sprintf(top_str, "%ld", UNTOP_TOPIC_TIME_VALUE);
	}

	if(!global_af_handler[rand_mh].pConn)
	{
		mysqlSimpConnect(r->pool, af_db_cfg, &global_af_handler[rand_mh], &errBuf);
	}
		
	snprintf(sql_str, sizeof(sql_str) - 1, "update af_topic_%s set lastPost = %s where rid = %d",
											sbi.id, top_str, int_root_topic_id);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);


	output_msgPage(r, facfg, ucocfg, desc_str, tmp_str, 1);
}





//hot or unhot topic
//hot_or_unhot 0:unhot 1:hot
void _topic_hot_unhot(request_rec *r, af_app_config* facfg, const char* board_bname,
					  signal_board_info sbi, apr_table_t* param_list, int hot_or_unhot)
{
	char sql_str[MEDIM_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	char tmp_str[MEDIM_MEM_SIZE] = {0};
	char desc_str[MEDIM_MEM_SIZE] = {0};
	char hot_str[SMALL_MEM_SIZE] = {0};
	const char* total_posts = 0;
	const char* root_topic_id = 0;
	int int_root_topic_id = 0;

	root_topic_id = ap_table_get(param_list, "t");
	int_root_topic_id = root_topic_id ? atoi(root_topic_id) : 0;

	if(int_root_topic_id <= 0)
	{
		//show invalid page
		output_invalid_page(r, facfg);
		return;
	}

	if(hot_or_unhot)
	{
		sprintf(desc_str, "set hot success");
	}
	else
	{
		sprintf(desc_str, "cancel hot success");
	}

	//relocation url
	snprintf(tmp_str, sizeof(tmp_str) - 1, "%s/%s?cmd=read&t=%d", facfg->appRunUrl, board_bname, int_root_topic_id);

	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	if(!global_af_handler[rand_mh].pConn)
	{
		mysqlSimpConnect(r->pool, af_db_cfg, &global_af_handler[rand_mh], &errBuf);
	}

	memset(sql_str, 0, sizeof(sql_str));

	snprintf(sql_str, sizeof(sql_str) - 1, "update af_topic_%s set hot = %d where id = %d",
											sbi.id, hot_or_unhot, int_root_topic_id);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);


	output_msgPage(r, facfg, ucocfg, desc_str, tmp_str, 1);
}



//lock or unlock topic
//lock_or_unlock 0:unlock 1:lock
void _topic_lock_unlock(request_rec *r, af_app_config* facfg, const char* board_bname,
					    signal_board_info sbi, apr_table_t* param_list, int lock_or_unlock)
{
	char sql_str[MEDIM_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	char tmp_str[MEDIM_MEM_SIZE] = {0};
	char desc_str[MEDIM_MEM_SIZE] = {0};
	char hot_str[SMALL_MEM_SIZE] = {0};
	const char* total_posts = 0;
	const char* root_topic_id = 0;
	int int_root_topic_id = 0;

	root_topic_id = ap_table_get(param_list, "t");
	int_root_topic_id = root_topic_id ? atoi(root_topic_id) : 0;

	if(int_root_topic_id <= 0)
	{
		//show invalid page
		output_invalid_page(r, facfg);
		return;
	}

	if(lock_or_unlock)
	{
		sprintf(desc_str, "set lock success");
	}
	else
	{
		sprintf(desc_str, "cancel lock success");
	}

	//relocation url
	snprintf(tmp_str, sizeof(tmp_str) - 1, "%s/%s?cmd=read&t=%d", facfg->appRunUrl, board_bname, int_root_topic_id);

	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	if(!global_af_handler[rand_mh].pConn)
	{
		mysqlSimpConnect(r->pool, af_db_cfg, &global_af_handler[rand_mh], &errBuf);
	}


	//update topic status
	memset(sql_str, 0, sizeof(sql_str));
	snprintf(sql_str, sizeof(sql_str) - 1, "update af_topic_%s set locked = %d where rid = %d",
											sbi.id, lock_or_unlock, int_root_topic_id);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	

	//update content status
	memset(sql_str, 0, sizeof(sql_str));
	snprintf(sql_str, sizeof(sql_str) - 1, "update af_content_%s set locked = %d where rid = %d",
											sbi.id, lock_or_unlock, int_root_topic_id);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	output_msgPage(r, facfg, ucocfg, desc_str, tmp_str, 1);
}

