/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name：fun_forum_read.c
* file desc：forum  read topic
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
#include "fun_forum_read.h"

//define topic read tpl files
#define FORUM_FLAT_TOPIC_READ_TPL "af_read_topic.html"
#define FORUM_TREE_TOPIC_READ_TPL "af_read_tree_topic.html"

#define FORUM_TOPIC_READ_LOGIN "af_read_topic_login.html"
#define FORUM_TOPIC_READ_POST  "af_read_topic_post.html"
#define FORUM_TOPIC_MANAGE_TPL "af_topic_manage.html"


//by flate method
void _read_topic_by_flat_type(request_rec *r, af_app_config* facfg,
							  const char* board_bname, signal_board_info sbi,
							  apr_table_t* param_list);

//by tree method
void _read_topic_by_tree_type(request_rec *r, af_app_config* facfg,
							  const char* board_bname, signal_board_info sbi,
							  apr_table_t* param_list);



//frefix tree topic by tree method
void _read_prefix_tree_topic(request_rec *r, af_app_config* facfg,
							 tpl_t **my_tpl, const char* board_bname,
							 const char* board_id, const char* root_topic_id,
							 const char* sub_topic_id);

//show quick login or post page
void _quick_login_post(request_rec *r, af_app_config* facfg, 
							  const char* board_bname,const char* root_topic_id,
							  const char* sub_topic_id, signal_board_info sbi,
							  const char* page, const char* topic_title);


//show topic manage page
void _topic_manage_page(request_rec *r, af_app_config* facfg,
						const char* board_bname, const char* root_topic_id,
						int is_toped, int is_locked, int is_hoted);


//reat signal topic
void topic_read(request_rec *r, af_app_config* facfg,
				const char* board_bname, signal_board_info sbi,
				apr_table_t* param_list)
{
	const char* topic_type = 0;
	int int_topic_type = 0;

	topic_type = ap_table_get(param_list, "s");
	int_topic_type = topic_type ? atoi(topic_type) : 1;

	if(int_topic_type == 2)
	{
		//by tree method
		_read_topic_by_tree_type(r, facfg, board_bname, sbi, param_list);
	}
	else
	{
		//by flate method
		_read_topic_by_flat_type(r, facfg, board_bname, sbi, param_list);
	}
}


//by flate method
void _read_topic_by_flat_type(request_rec *r, af_app_config* facfg,
							  const char* board_bname, signal_board_info sbi,
							  apr_table_t* param_list)
{
	tpl_t *my_tpl;
	char* content = 0;

	signal_root_topic_info srti = {0};//root tpic info

	char sql_str[MEDIM_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	const char* root_topic_id = 0;
	int int_root_topic_id = 0;

	const char* page = 0;
	int int_page = 0;
	const char* sub_page = 0;
	int int_sub_page = 0;
	int offset = 0;
	int rec_per_page = 0;

	char tmp_str[SMALL_MEM_SIZE] = {0};

	//get param
	page = ap_table_get(param_list, "p");//page no
	int_page = (page && strcmp(page, "(null)")) ? atoi(page) : 1;

	if(int_page <= 0)
	{
		int_page = 1;
	}
	
	sub_page = ap_table_get(param_list, "sp");//sub page
	int_sub_page = (sub_page && strcmp(sub_page, "(null)")) ? atoi(sub_page) : 1;

	if(int_sub_page <= 0)
	{
		int_sub_page = 1;
	}

	rec_per_page = facfg->appRecSubPage ? atoi(facfg->appRecSubPage) : 10;
	offset = (int_sub_page - 1) * rec_per_page;

	root_topic_id = ap_table_get(param_list, "t");
	int_root_topic_id = root_topic_id ? atoi(root_topic_id) : 0;

	
	if(int_root_topic_id <= 0)
	{
		//show invalid page
		output_invalid_page(r, facfg);
		return;
	}

	//get root topic
	ret = get_signal_root_topic_info(r, atoi(sbi.id), int_root_topic_id, &srti);

	if(!ret)
	{
		//show invalid page
		output_invalid_page(r, facfg);
		return;
	}

	//relocation url
	snprintf(tmp_str, sizeof(tmp_str) - 1, "%s/%s", facfg->appRunUrl, board_bname);

	//show page header
	output_pageHeader(r, facfg, srti.title, ucocfg);
	

	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, FORUM_TPL_DIR, FORUM_FLAT_TOPIC_READ_TPL);

	//set general variable
	gen_pub_varible_set(r, facfg, &my_tpl);
	
	tpl_set_field_fmt_global(my_tpl, "show_board_bname", "%s", board_bname ? board_bname : sbi.id);
	tpl_set_field_fmt_global(my_tpl, "show_board_name", "%s", sbi.name ? sbi.name : "");
	tpl_set_field_fmt_global(my_tpl, "show_root_topic_id", "%s", root_topic_id ? root_topic_id : "");
	tpl_set_field_fmt_global(my_tpl, "show_rootTopic_title", "%s", srti.title ? srti.title : "");
	tpl_set_field_fmt_global(my_tpl, "show_cur_page", "%s", page ? page : "1");


	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	if(!global_af_handler[rand_mh].pConn)
	{
		mysqlSimpConnect(r->pool, af_db_cfg, &global_af_handler[rand_mh], &errBuf);
	}

	//mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], "SET NAMES 'GB2312'", &errBuf);
	memset(sql_str, 0, sizeof(sql_str));
	snprintf(sql_str, sizeof(sql_str) - 1, "select t.id, t.title, t.express, t.content, t.posterId, t.attList, m.name, m.regDate, \
					  m.myPhoto, m.posts from af_content_%s t left join af_member m on t.posterId = m.id where t.rid = %d limit %d, %d", 
					  sbi.id, int_root_topic_id, offset ,rec_per_page);

	//ap_rprintf(r, "<h1>%s</h1>", sql_str);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	if(!ret)
	{
		//read topic faild
		output_msgPage(r, facfg, ucocfg, "read topic failed.", tmp_str, 0);
		return;
	}

	rec_num = mysqlNumRows(&global_af_handler[rand_mh]);

	if(rec_num > 0)
	{
		tpl_select_section(my_tpl, "topiclist");

		while(global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]))
		{

			tpl_set_field_fmt(my_tpl, "show_board_bname", "%s", board_bname);
			tpl_set_field_fmt(my_tpl, "show_rootTopic_id", "%s", root_topic_id);
			
			
			//topic id
			tpl_set_field_fmt(my_tpl, "show_topic_id", "%s", global_af_handler[rand_mh].pRow[0]);
			
			//title
			tpl_set_field_fmt(my_tpl, "show_topic_title", "%s", global_af_handler[rand_mh].pRow[1]);
			
			//express
			tpl_set_field_fmt(my_tpl, "show_topic_express", "%s", global_af_handler[rand_mh].pRow[2] ? global_af_handler[rand_mh].pRow[2] : "");
			
			//content
			tpl_set_field_fmt(my_tpl, "show_topic_content", "%s", global_af_handler[rand_mh].pRow[3] ? global_af_handler[rand_mh].pRow[3] : "");
			
			//posterId
			tpl_set_field_fmt(my_tpl, "show_poster_id", "%s", global_af_handler[rand_mh].pRow[4] ? global_af_handler[rand_mh].pRow[4] : "");

			//img url
			memset(tmp_str, 0, sizeof(tmp_str));
			if(strlen(global_af_handler[rand_mh].pRow[5]) > 4)
			{
				snprintf(tmp_str, sizeof(tmp_str) - 1, "<img src=%s><br/>", global_af_handler[rand_mh].pRow[5]);
			}
			tpl_set_field_fmt(my_tpl, "show_topic_img", "%s",tmp_str);

			
			//topic poster
			tpl_set_field_fmt(my_tpl, "show_poster", "%s", global_af_handler[rand_mh].pRow[6] ? global_af_handler[rand_mh].pRow[6] : "");
			
			//poster reg date
			tpl_set_field_fmt(my_tpl, "poster_reg_date", "%s", global_af_handler[rand_mh].pRow[7] ? global_af_handler[rand_mh].pRow[7] : "");
			
			//poster logo
			memset(tmp_str, 0, sizeof(tmp_str));
			if(strlen(global_af_handler[rand_mh].pRow[7]) > 4)
			{
				snprintf(tmp_str, sizeof(tmp_str) - 1, "<img src=%s/%s>", facfg->appUploadUrl, global_af_handler[rand_mh].pRow[8]);
				tpl_set_field_fmt(my_tpl, "show_poster_logo", "%s",tmp_str);
			}

			//poster posts
			tpl_set_field_fmt(my_tpl, "show_poster_posts", "%s", global_af_handler[rand_mh].pRow[9] ? global_af_handler[rand_mh].pRow[9] : "0");
			
			tpl_append_section(my_tpl);
		}
		
		//free result
		mysqlFreeResult(&global_af_handler[rand_mh]);

		tpl_deselect_section(my_tpl);
	}

	//update man topic hit number
	memset(sql_str, 0, sizeof(sql_str));
	snprintf(sql_str, sizeof(sql_str) - 1, "update af_topic_%s set hit = hit + 1 where id = %d", sbi.id, int_root_topic_id);
	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	//general page list
	memset(tmp_str, 0, sizeof(tmp_str));
	snprintf(tmp_str, sizeof(tmp_str) - 1, "%s?cmd=read&s=1&t=%d&p=%d&sp=", board_bname, int_root_topic_id, int_page);
	general_page_list(r, &my_tpl, tmp_str, 1, srti.reply, rec_per_page, int_sub_page);
		
	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);


	//load topic manage menu
	if(ucocfg.type == 4)
	{
		_topic_manage_page(r, facfg, board_bname, root_topic_id, srti.toped, srti.locked, srti.hoted);
	}

	
	//show quick login or post page
	_quick_login_post(r, facfg, board_bname, root_topic_id, "0", sbi, page, srti.title);
	
	//ouput page bottom
	output_pageBottom(r, facfg);

}


//by tree method
void _read_topic_by_tree_type(request_rec *r, af_app_config* facfg,
							  const char* board_bname, signal_board_info sbi,
							  apr_table_t* param_list)
{
	tpl_t *my_tpl;
	char* content = 0;

	char tmp_str[SMALL_MEM_SIZE] = {0};
	char sql_str[MEDIM_MEM_SIZE] = {0};

	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	const char* page = 0;
	int int_page = 0;

	const char* cur_topic_title = 0;
	const char* root_topic_id = 0;
	const char* sub_topic_id = 0;

	int int_root_topic_id = 0;
	int in_sub_topic_id = 0;

	
	signal_root_topic_info srti = {0};//root tpic info

	//get param
	page = ap_table_get(param_list, "p");
	int_page = (page && strcmp(page, "(null)")) ? atoi(page) : 1;

	root_topic_id = ap_table_get(param_list, "t");
	int_root_topic_id = root_topic_id ? atoi(root_topic_id) : 0;

	sub_topic_id = ap_table_get(param_list, "i");
	in_sub_topic_id = sub_topic_id ? atoi(sub_topic_id) : 0;

	//ap_set_content_type(r, "text/html");

	if(int_root_topic_id <= 0 || in_sub_topic_id <= 0)
	{
		//show invalid page
		output_invalid_page(r, facfg);
		return;
	}

	//page relocation url
	sprintf(tmp_str, "%s/board/%s", facfg->appRunUrl, board_bname);

	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, FORUM_TPL_DIR, FORUM_TREE_TOPIC_READ_TPL);

	//set general variable
	gen_pub_varible_set(r, facfg, &my_tpl);
	
	tpl_set_field_fmt_global(my_tpl, "show_board_bname", "%s", board_bname ? board_bname : sbi.id);
	tpl_set_field_fmt_global(my_tpl, "show_board_name", "%s", sbi.name ? sbi.name : "");
	tpl_set_field_fmt_global(my_tpl, "show_root_topic_id", "%s", root_topic_id ? root_topic_id : "");
	
	tpl_set_field_fmt_global(my_tpl, "show_cur_topic_id", "%s",  sub_topic_id);
	tpl_set_field_fmt_global(my_tpl, "show_cur_page", "%s", page);


	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	if(!global_af_handler[rand_mh].pConn)
	{
		mysqlSimpConnect(r->pool, af_db_cfg, &global_af_handler[rand_mh], &errBuf);
	}

	//mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], "SET NAMES 'GB2312'", &errBuf);

	memset(sql_str, 0, sizeof(sql_str));
	sprintf(sql_str, "select t.id, t.title, t.express, t.content, t.postTime, \
					  t.posterId, t.attList, m.name, m.regDate, m.myPhoto, m.posts from af_content_%s t \
					  left join af_member m on t.posterId = m.id where t.id = %d limit 1",
					  sbi.id, in_sub_topic_id);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	if(!ret)
	{
		//错误页面重定向
		output_msgPage(r, facfg, ucocfg, "read topic failed.", tmp_str, 0);
		return;
	}

	rec_num = mysqlNumRows(&global_af_handler[rand_mh]);

	if(rec_num <= 0)
	{
		//read topic faild
		output_msgPage(r, facfg, ucocfg, "read topic failed.", tmp_str, 0);
		return;
	}

	//get signal record
	global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]);

	
	//content
	tpl_set_field_fmt_global(my_tpl, "show_current_content", "%s", global_af_handler[rand_mh].pRow[3] ? global_af_handler[rand_mh].pRow[3] : "");

	//img url
	memset(tmp_str, 0, sizeof(tmp_str));
	if(strlen(global_af_handler[rand_mh].pRow[6]) > 4)
	{
		snprintf(tmp_str, sizeof(tmp_str) - 1, "<img src=%s><br>", global_af_handler[rand_mh].pRow[6]);
	}
	tpl_set_field_fmt(my_tpl, "show_topic_img", "%s",tmp_str);

	//poster
	tpl_set_field_fmt_global(my_tpl, "show_current_poster", "%s", global_af_handler[rand_mh].pRow[7] ? global_af_handler[rand_mh].pRow[7] : "");

	//poster regdate
	tpl_set_field_fmt_global(my_tpl, "poster_reg_date", "%s", global_af_handler[rand_mh].pRow[8] ? global_af_handler[rand_mh].pRow[8] : "");	

	
	//poster logo
	memset(tmp_str, 0, sizeof(tmp_str));
	if(strlen(global_af_handler[rand_mh].pRow[9]) > 4)
	{
		snprintf(tmp_str, sizeof(tmp_str) - 1, "<img src=%s/%s>", facfg->appUploadUrl, global_af_handler[rand_mh].pRow[9]);
		tpl_set_field_fmt(my_tpl, "show_current_poster_logo", "%s",tmp_str);
	}

	//poster posts
	tpl_set_field_fmt(my_tpl, "show_current_poster_posts", "%s", global_af_handler[rand_mh].pRow[10] ? global_af_handler[rand_mh].pRow[10] : "0");	
		
	//title
	cur_topic_title = global_af_handler[rand_mh].pRow[1];
	sprintf(tmp_str, "%s", cur_topic_title);
	tpl_set_field_fmt_global(my_tpl, "show_current_title", "%s", tmp_str);



	//show page header
	output_pageHeader(r, facfg, tmp_str, ucocfg);

	
	//free result
	mysqlFreeResult(&global_af_handler[rand_mh]);

	//update current topic hit number
	memset(sql_str, 0, sizeof(sql_str));
	snprintf(sql_str, sizeof(sql_str) - 1, "update af_topic_%s set hit = hit + 1 where id = %d", sbi.id, in_sub_topic_id);
	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	//update root topic hit number
	memset(sql_str, 0, sizeof(sql_str));
	snprintf(sql_str, sizeof(sql_str) - 1, "update af_topic_%s set hit = hit + 1 where id = %d", sbi.id, int_root_topic_id);
	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);
	

	//ap_rprintf(r, "<h1>%s, ret:%d</h1>", sql_str, ret);

	//get prefix tree topic
	_read_prefix_tree_topic(r, facfg, &my_tpl, board_bname, sbi.id, root_topic_id, sub_topic_id);

	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);

	
	//load topic manage menu
	if(ucocfg.type == 4)
	{	
		//get root topic
		ret = get_signal_root_topic_info(r, atoi(sbi.id), int_root_topic_id, &srti);

		_topic_manage_page(r, facfg, board_bname, root_topic_id, srti.toped, srti.locked, srti.hoted);
	}
	
	
	//show quick login or post page
	_quick_login_post(r, facfg, board_bname, root_topic_id, sub_topic_id, sbi, page, tmp_str);

	
	//ouput page bottom
	output_pageBottom(r, facfg);
}



//frefix tree topic by tree method
void _read_prefix_tree_topic(request_rec *r, af_app_config* facfg,
							 tpl_t **my_tpl, const char* board_bname,
							 const char* board_id, const char* root_topic_id, const char* sub_topic_id)
{
	char tmp_str[MEDIM_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	const char* extimg = 0;
	const char* hasAtt = 0;
	const char* lay = 0;
	const char* postTime = 0;
	char space_width[SMALL_MEM_SIZE] = {0};

	const char* cur_topic_id = 0;
	const char* cur_topic_title = 0;

	if(!root_topic_id || !(*root_topic_id))
	{
		return;
	}

	if(!sub_topic_id || !(*sub_topic_id))
	{
		return;
	}

	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	if(!global_af_handler[rand_mh].pConn)
	{
		mysqlSimpConnect(r->pool, af_db_cfg, &global_af_handler[rand_mh], &errBuf);
	}

	//mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], "SET NAMES 'GB2312'", &errBuf);
	memset(tmp_str, 0, sizeof(tmp_str));
	sprintf(tmp_str, "select id, express, title, len, poster, posterId, \
					  rid, lay, attType, postTime, hit  from af_topic_%s where tt = 1 \
					  and rid in(%s) order by lastPost desc, \
					  ord asc", board_id, root_topic_id);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], tmp_str, &errBuf);

	if(!ret)
	{
		return;
	}
	
	rec_num = mysqlNumRows(&global_af_handler[rand_mh]);

	if(rec_num <= 0)
	{
		return;
	}

	tpl_select_section(*my_tpl, "fixtopiclist");
	
	while(global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]))
	{
		memset(tmp_str, 0, sizeof(tmp_str));
		
		tpl_set_field_fmt(*my_tpl, "show_board_bname", "%s", board_bname);
		
		//topicId
		cur_topic_id = global_af_handler[rand_mh].pRow[0];
		tpl_set_field_fmt(*my_tpl, "show_topic_id", "%s", cur_topic_id ? cur_topic_id : "");
		
		//express
		extimg = global_af_handler[rand_mh].pRow[1];
		
		if(extimg && atoi(extimg) > 0)
		{
			//has express
			memset(tmp_str, 0, sizeof(tmp_str));
			sprintf(tmp_str, "<img src=\"%s/images/smiles/%s.gif\" border=\"0\">", 
				facfg->appUrl , extimg);
			tpl_set_field_fmt(*my_tpl, "show_express", "%s", tmp_str);
		}
		else
		{
			tpl_set_field_fmt(*my_tpl, "show_express", "%s", "");
		}
		
		//title
		cur_topic_title = global_af_handler[rand_mh].pRow[2];

		if(atoi(cur_topic_id) == atoi(sub_topic_id))
		{
			sprintf(tmp_str, "<font class='curTopic'>%s</font>", cur_topic_title);
			tpl_set_field_fmt(*my_tpl, "show_topic_title", "%s", tmp_str);
		}
		else
		{
			tpl_set_field_fmt(*my_tpl, "show_topic_title", "%s", cur_topic_title ? cur_topic_title : "");
		}
		
		//topicLen
		tpl_set_field_fmt(*my_tpl, "show_topic_len", "%s", global_af_handler[rand_mh].pRow[3] ? global_af_handler[rand_mh].pRow[3] : "0");
		
		//poster
		tpl_set_field_fmt(*my_tpl, "show_poster", "%s", global_af_handler[rand_mh].pRow[4] ? global_af_handler[rand_mh].pRow[4] : "");
		
		//posterId
		tpl_set_field_fmt(*my_tpl, "show_poster_id", "%s", global_af_handler[rand_mh].pRow[5] ? global_af_handler[rand_mh].pRow[5] : "");

		
		//rootTopicId
		tpl_set_field_fmt(*my_tpl, "show_rootTopic_id", "%s", global_af_handler[rand_mh].pRow[6] ? global_af_handler[rand_mh].pRow[6] : "0");
		
		//space with, by lay
		lay = global_af_handler[rand_mh].pRow[7];
		memset(space_width, 0, sizeof(space_width));
		sprintf(space_width, "%d", lay ? atol(lay)*30 : 0);
		
		tpl_set_field_fmt(*my_tpl, "show_space_width", "%s", space_width);
		
		//li style			
		if(lay && atol(lay) == 0)
		{
			tpl_set_field_fmt(*my_tpl, "show_li_style", "%s", "rect");
		}
		else if(lay && atol(lay) == 1)
		{
			tpl_set_field_fmt(*my_tpl, "show_li_style", "%s", "circle");
		}
		else
		{
			tpl_set_field_fmt(*my_tpl, "show_li_style", "%s", "square");
		}
		
		//has attachment
		hasAtt = global_af_handler[rand_mh].pRow[8];
		memset(tmp_str, 0, sizeof(tmp_str));
		if(hasAtt && atoi(hasAtt) == 1)
		{
			sprintf(tmp_str, "<img src=\"%s/images/att_img.gif\">", facfg->appUrl);
		}

		tpl_set_field_fmt(*my_tpl, "show_att_type", "%s", tmp_str);
		
		//post time
		postTime = global_af_handler[rand_mh].pRow[9];
		tpl_set_field_fmt(*my_tpl, "show_post_time", "%s", postTime ? postTime : "");
		
		//hit number
		tpl_set_field_fmt(*my_tpl, "show_topic_hits", "%s", 
						  global_af_handler[rand_mh].pRow[10] ? global_af_handler[rand_mh].pRow[10] : "0");

		tpl_append_section(*my_tpl);
	}

	//free result
	mysqlFreeResult(&global_af_handler[rand_mh]);
}


//show quick login or post page
void _quick_login_post(request_rec *r, af_app_config* facfg, 
							  const char* board_bname,const char* root_topic_id,
							  const char* sub_topic_id, signal_board_info sbi,
							  const char* page, const char* topic_title)
{
	tpl_t *my_tpl;
	char* content = 0;
	char tpl_file[SMALL_MEM_SIZE] = {0};


	//check user is logined?
	if(ucocfg.available)
	{
		sprintf(tpl_file, "%s", FORUM_TOPIC_READ_POST);
	}
	else
	{
		sprintf(tpl_file, "%s", FORUM_TOPIC_READ_LOGIN);
	}

	
	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, FORUM_TPL_DIR, tpl_file);

	//set general variable
	gen_pub_varible_set(r, facfg, &my_tpl);
	
	tpl_set_field_fmt_global(my_tpl, "show_board_bname", "%s", board_bname ? board_bname : sbi.id);
	tpl_set_field_fmt_global(my_tpl, "show_board_name", "%s", sbi.name ? sbi.name : "");
	tpl_set_field_fmt_global(my_tpl, "show_root_topic_id", "%s", root_topic_id ? root_topic_id : "");
	
	tpl_set_field_fmt_global(my_tpl, "show_cur_topic_id", "%s", sub_topic_id);
	tpl_set_field_fmt_global(my_tpl, "show_cur_page", "%s", page);
	tpl_set_field_fmt_global(my_tpl, "show_current_title", "%s", topic_title);

	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);
}



//show topic manage page
void _topic_manage_page(request_rec *r, af_app_config* facfg,
						const char* board_bname, const char* root_topic_id,
						int is_toped, int is_locked, int is_hoted)
{
	tpl_t *my_tpl;
	char* content = 0;
	char tmp_str[SMALL_MEM_SIZE] = {0};
	char tmp_str2[SMALL_MEM_SIZE] = {0};

	
	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, FORUM_TPL_DIR, FORUM_TOPIC_MANAGE_TPL);

	//set general variable
	gen_pub_varible_set(r, facfg, &my_tpl);
	
	tpl_set_field_fmt_global(my_tpl, "show_board_bname", "%s", board_bname);
	tpl_set_field_fmt_global(my_tpl, "show_topic_id", "%s", root_topic_id ? root_topic_id : "");
	
	//toped?
	memset(tmp_str, 0, sizeof(tmp_str));
	memset(tmp_str2, 0, sizeof(tmp_str2));

	if(is_toped)
	{
		sprintf(tmp_str, "%s", "Cancel Top");
		sprintf(tmp_str2, "%s", "utop");
	}
	else
	{
		sprintf(tmp_str, "%s", "Top");
		sprintf(tmp_str2, "%s", "top");
	}
	tpl_set_field_fmt_global(my_tpl, "top_desc", "%s", tmp_str);
	tpl_set_field_fmt_global(my_tpl, "top_cmd", "%s", tmp_str2);


	//locked?
	memset(tmp_str, 0, sizeof(tmp_str));
	memset(tmp_str2, 0, sizeof(tmp_str2));

	if(is_locked)
	{
		sprintf(tmp_str, "%s", "Cancel Lock");
		sprintf(tmp_str2, "%s", "ulock");
	}
	else
	{
		sprintf(tmp_str, "%s", "Lock");
		sprintf(tmp_str2, "%s", "lock");
	}
	tpl_set_field_fmt_global(my_tpl, "lock_desc", "%s", tmp_str);
	tpl_set_field_fmt_global(my_tpl, "lock_cmd", "%s", tmp_str2);


	//hoted?
	memset(tmp_str, 0, sizeof(tmp_str));
	memset(tmp_str2, 0, sizeof(tmp_str2));

	if(is_hoted)
	{
		sprintf(tmp_str, "%s", "Cancel Hot");
		sprintf(tmp_str2, "%s", "uhot");
	}
	else
	{
		sprintf(tmp_str, "%s", "Hot");
		sprintf(tmp_str2, "%s", "hot");
	}
	tpl_set_field_fmt_global(my_tpl, "hot_desc", "%s", tmp_str);
	tpl_set_field_fmt_global(my_tpl, "hot_cmd", "%s", tmp_str2);


	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);
}
