/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name：fun_forum_topic.c
* file desc：forum signal board topic list
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


#include "apr_file_io.h"
#include "apr_file_info.h"

#include "apache_request.h"
#include "fun_cfg.h"
#include "tpllib.h"
#include "fun_public.h"

#include "fun_forum_public.h"
#include "fun_forum_topic.h"


//define topic tpl files
#define FORUM_FLAT_TOPIC_LIST_TPL "af_thread_list.html"
#define FORUM_TREE_TOPIC_LIST_TPL "af_tree_list.html"



//show topic by flate type
void list_topic_flat_type(request_rec *r, af_app_config* facfg,
						  const char* board_bname, signal_board_info sbi,
						  apr_table_t* sub_param_list);

//show topic by tree type
void list_topic_tree_type(request_rec *r, af_app_config* facfg,
						  const char* board_bname, signal_board_info sbi,
						  apr_table_t* sub_param_list);


//topic list
void list_topic(request_rec *r, af_app_config* facfg,
				const char* board_bname, signal_board_info sbi,
				apr_table_t* param_list)
{
	const char* topic_list_type = 0;
	int int_topic_list_type = 0;
	
	///get suit parameter value
	topic_list_type = ap_table_get(param_list, "s");//topic list type
	int_topic_list_type = topic_list_type ? atoi(topic_list_type) : 1;
	
	//show page header
	output_pageHeader(r, facfg, sbi.name,  ucocfg);

	//show topic by two method
	if(int_topic_list_type == 2)
	{
		//tree type
		list_topic_tree_type(r, facfg, board_bname, sbi, param_list);
	}
	else
	{
		//flate type
		list_topic_flat_type(r, facfg, board_bname, sbi, param_list);
	}

	//ouput page bottom
	output_pageBottom(r, facfg);
}



//show topic by flate type
void list_topic_flat_type(request_rec *r, af_app_config* facfg,
						  const char* board_bname, signal_board_info sbi,
						  apr_table_t* sub_param_list)
{	
	tpl_t* my_tpl;
	char* content = 0;
	
	char tmp_str[MEDIM_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int ret = 0;
	int rec_num = 0;

	const char* title = 0;
	const char* eximg = 0;
	char express_img[SMALL_MEM_SIZE] = {0};
	char opt_url[SMALL_MEM_SIZE] = {0};
	char order_by_sql[SMALL_MEM_SIZE] = {0};
	char order_by_css[SMALL_MEM_SIZE] = {0};

	const char* page = 0;
	const char* list_type = 0;
	int int_page = 0;
	int int_list_type = 0;
	int offset = 0;
	int rec_per_page = 0;

	apr_status_t rv;
	apr_file_t *fp = NULL;

	//get current param
	page = ap_table_get(sub_param_list, "p");
	list_type = ap_table_get(sub_param_list, "l");//1:by time 2:by hits 3:by replies

	int_page = (page && strcmp(page, "(null)")) ? atoi(page) : 1;
	int_list_type = (list_type && strcmp(list_type, "(null)")) ? atoi(list_type) : 0;

	if(int_page <= 0)
	{
		int_page = 1;
	}

	snprintf(order_by_css, sizeof(order_by_css) - 1, "%s", "class='curFocus'");


	rec_per_page = facfg->appRecPePage ? atoi(facfg->appRecPePage) : 15;
	offset = (int_page - 1) * rec_per_page;

	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, FORUM_TPL_DIR, FORUM_FLAT_TOPIC_LIST_TPL);

	//set general variable
	gen_pub_varible_set(r, facfg, &my_tpl);
	tpl_set_field_fmt_global(my_tpl, "show_board_bname", "%s", board_bname ? board_bname : sbi.id);
	tpl_set_field_fmt_global(my_tpl, "show_board_name", "%s", sbi.name ? sbi.name : "");
	tpl_set_field_fmt_global(my_tpl, "show_board_intro", "%s", sbi.intro ? sbi.intro : "");
	tpl_set_field_fmt_global(my_tpl, "show_cur_page", "%s", page ? page : "1");

	
	//sub boards select js
	memset(tmp_str, 0, sizeof(tmp_str));
	snprintf(tmp_str, sizeof(tmp_str) - 1, "%s/af_boards.js", facfg->appUploadPath);

	r->filename = ap_psprintf(r->pool, tmp_str);	
	rv = apr_file_open(&fp, r->filename, APR_READ, APR_UREAD | APR_GREAD, r->pool);
	
	if(rv == APR_SUCCESS)
	{
		memset(tmp_str, 0, sizeof(tmp_str));
		snprintf(tmp_str, sizeof(tmp_str) - 1, "%s/af_boards.js", facfg->appUploadUrl);
		tpl_set_field_fmt_global(my_tpl, "sub_board_js", "%s", tmp_str);
	}

	switch(int_list_type)
	{
		case 2:
		snprintf(order_by_sql, sizeof(order_by_sql) - 1, "%s", "hit");
		tpl_set_field_fmt_global(my_tpl, "by_hit", "%s", order_by_css);
		break;

		case 3:
		snprintf(order_by_sql, sizeof(order_by_sql) - 1, "%s", "reply");
		tpl_set_field_fmt_global(my_tpl, "by_reply", "%s", order_by_css);
		break;
		
		default:
		snprintf(order_by_sql, sizeof(order_by_sql) - 1, "%s", "lastPost");
		tpl_set_field_fmt_global(my_tpl, "by_time", "%s", order_by_css);
		break;
	}


	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	//get topic data
	//mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], "SET NAMES 'GB2312'", &errBuf);

	if(!global_af_handler[rand_mh].pConn)
	{
		mysqlSimpConnect(r->pool, af_db_cfg, &global_af_handler[rand_mh], &errBuf);
	}

	memset(tmp_str, 0, sizeof(tmp_str));
	snprintf(tmp_str, sizeof(tmp_str) - 1, "select id, title, express, hit, reply, poster, posterId, \
					  lastPoster, lastPosterId, postTime, hot, locked, lastPost, attType from af_topic_%s \
					  where ft = 1 order by %s desc \
					  limit %d, %d", sbi.id, order_by_sql, offset, rec_per_page);
	
	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], tmp_str, &errBuf);

	//ap_set_content_type(r, "text/html");
	//ap_rprintf(r, "%s", sql_str);
	//ap_rprintf(r, "<h1>ret:%d, %s</h1>", ret, errBuf);

	if(ret)
	{
		rec_num = mysqlNumRows(&global_af_handler[rand_mh]);
		
		if(rec_num > 0)
		{
			tpl_select_section(my_tpl, "threadlist");

			while(global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]))
			{
				memset(express_img, 0, sizeof(express_img));
				eximg = global_af_handler[rand_mh].pRow[2];
				
				if(eximg && atoi(eximg) > 0)
				{
					sprintf(express_img, "<img src='%s/images/smiles/%s.gif'>", facfg->appUrl, eximg);
				}
				
				tpl_set_field_fmt(my_tpl, "show_express", "%s", express_img);
				tpl_set_field_fmt(my_tpl, "show_topic_id", "%s", global_af_handler[rand_mh].pRow[0] ? global_af_handler[rand_mh].pRow[0] : "");
				
				
				title = global_af_handler[rand_mh].pRow[1];
				tpl_set_field_fmt(my_tpl, "show_topic_title", "%s", title ? title : "");
				
				tpl_set_field_fmt(my_tpl, "show_hit_num", "%s", global_af_handler[rand_mh].pRow[3] ? global_af_handler[rand_mh].pRow[3] : "0");
				tpl_set_field_fmt(my_tpl, "show_reply_num", "%s", global_af_handler[rand_mh].pRow[4] ? global_af_handler[rand_mh].pRow[4] : "0");
				
				
				tpl_set_field_fmt(my_tpl, "show_poster", "%s", global_af_handler[rand_mh].pRow[5] ? global_af_handler[rand_mh].pRow[5] : "");
				tpl_set_field_fmt(my_tpl, "show_poster_id", "%s", global_af_handler[rand_mh].pRow[6] ? global_af_handler[rand_mh].pRow[6] : "");
				tpl_set_field_fmt(my_tpl, "show_last_poster", "%s", global_af_handler[rand_mh].pRow[7] ? global_af_handler[rand_mh].pRow[7] : "");
				tpl_set_field_fmt(my_tpl, "show_last_poster_id", "%s", global_af_handler[rand_mh].pRow[8] ? global_af_handler[rand_mh].pRow[8] : "");
				tpl_set_field_fmt(my_tpl, "last_post_time", "%s", global_af_handler[rand_mh].pRow[9] ? global_af_handler[rand_mh].pRow[9] : "");



				//hot img
				memset(tmp_str, 0, sizeof(tmp_str));

				if(atoi(global_af_handler[rand_mh].pRow[10]) == 1)
				{
					snprintf(tmp_str, sizeof(tmp_str) - 1, "<img src=%s/images/hot.gif alt='Hot'>", facfg->appUrl);
				}

				tpl_set_field_fmt(my_tpl, "show_hot_img", "%s", tmp_str);

				//lock img
				memset(tmp_str, 0, sizeof(tmp_str));

				if(atoi(global_af_handler[rand_mh].pRow[11]) == 1)
				{
					snprintf(tmp_str, sizeof(tmp_str) - 1, "<img src=%s/images/lock.gif alt='Lock'>", facfg->appUrl);
				}

				tpl_set_field_fmt(my_tpl, "show_lock_img", "%s", tmp_str);

				//toped img
				memset(tmp_str, 0, sizeof(tmp_str));

				if(atol(global_af_handler[rand_mh].pRow[12]) == TOP_TOPIC_TIME_VALUE)
				{
					snprintf(tmp_str, sizeof(tmp_str) - 1, "<img src=%s/images/top.gif alt='Top'>", facfg->appUrl);
				}

				tpl_set_field_fmt(my_tpl, "show_top_img", "%s", tmp_str);

				
				//attach img
				memset(tmp_str, 0, sizeof(tmp_str));

				if(atoi(global_af_handler[rand_mh].pRow[13]) == 1)
				{
					snprintf(tmp_str, sizeof(tmp_str) - 1, "<img src=%s/images/att_img.gif>", facfg->appUrl);
				}

				tpl_set_field_fmt(my_tpl, "show_att_type", "%s", tmp_str);


				/////////////
				tpl_append_section(my_tpl);
			}
			
			//free result
			mysqlFreeResult(&global_af_handler[rand_mh]);

			tpl_deselect_section(my_tpl);
		}
	}

	//general page list
	sprintf(opt_url, "%s?s=1&p=", board_bname);
	general_page_list(r, &my_tpl, opt_url, 1, atoi(sbi.topics), rec_per_page, int_page);

	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);
	
}


//show topic by tree type
void list_topic_tree_type(request_rec *r, af_app_config* facfg,
						  const char* board_bname, signal_board_info sbi,
						  apr_table_t* sub_param_list)
{
	tpl_t *my_tpl;
	char* content = 0;

	char tmp_str[MEDIM_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	const char* extimg = 0;
	const char* hasAtt = 0;
	const char* lay = 0;
	const char* postTime = 0;

	char tmp_str1[SMALL_MEM_SIZE] = {0};
	char space_width[SMALL_MEM_SIZE] = {0};
	char order_by_sql[SMALL_MEM_SIZE] = {0};
	char *page_no = NULL;

	char rootTopicIdList[MEDIM_MEM_SIZE] = {0};
	char order_by_css[SMALL_MEM_SIZE] = {0};
	int i = 0;

	const char* page = 0;
	const char* list_type = 0;
	int int_page = 0;
	int int_list_type = 0;
	int offset = 0;
	int rec_per_page = 0;
	char opt_url[SMALL_MEM_SIZE] = {0};

	apr_status_t rv;
	apr_file_t *fp = NULL;
	
	//r->content_type = "text/html";
	//ap_send_http_header(r);

	//get current param
	page = ap_table_get(sub_param_list, "p");
	list_type = ap_table_get(sub_param_list, "l");//1:by time 2:by hits 3:by replies

	int_page = (page && strcmp(page, "(null)")) ? atoi(page) : 1;
	int_list_type = (list_type && strcmp(list_type, "(null)")) ? atoi(list_type) : 0;

	if(int_page <= 0)
	{
		int_page = 1;
	}

	snprintf(order_by_css, sizeof(order_by_css) - 1, "%s", "class='curFocus'");

	rec_per_page = facfg->appRecPePage ? atoi(facfg->appRecPePage) : 15;
	offset = (int_page - 1) * rec_per_page;

	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, FORUM_TPL_DIR, FORUM_TREE_TOPIC_LIST_TPL);

	//set general variable
	gen_pub_varible_set(r, facfg, &my_tpl);
	tpl_set_field_fmt_global(my_tpl, "show_board_bname", "%s", board_bname ? board_bname : sbi.id);
	tpl_set_field_fmt_global(my_tpl, "show_board_name", "%s", sbi.name ? sbi.name : "");
	tpl_set_field_fmt_global(my_tpl, "show_board_intro", "%s", sbi.intro ? sbi.intro : "");
	tpl_set_field_fmt_global(my_tpl, "show_cur_page", "%s", page ? page : "1");

	//sub boards select js
	memset(tmp_str, 0, sizeof(tmp_str));
	snprintf(tmp_str, sizeof(tmp_str) - 1, "%s/af_boards.js", facfg->appUploadPath);
	
	r->filename = ap_psprintf(r->pool, tmp_str);	
	rv = apr_file_open(&fp, r->filename, APR_READ, APR_UREAD | APR_GREAD, r->pool);
	
	if(rv == APR_SUCCESS)
	{
		memset(tmp_str, 0, sizeof(tmp_str));
		snprintf(tmp_str, sizeof(tmp_str) - 1, "%s/af_boards.js", facfg->appUploadUrl);
		tpl_set_field_fmt_global(my_tpl, "sub_board_js", "%s", tmp_str);
	}

	
	switch(int_list_type)
	{
		case 2:
		snprintf(order_by_sql, sizeof(order_by_sql) - 1, "%s", "hit");
		tpl_set_field_fmt_global(my_tpl, "by_hit", "%s", order_by_css);
		break;

		case 3:
		snprintf(order_by_sql, sizeof(order_by_sql) - 1, "%s", "reply");
		tpl_set_field_fmt_global(my_tpl, "by_reply", "%s", order_by_css);
		break;
		
		default:
		snprintf(order_by_sql, sizeof(order_by_sql) - 1, "%s", "lastPost");
		tpl_set_field_fmt_global(my_tpl, "by_time", "%s", order_by_css);
		break;
	}


	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	if(!global_af_handler[rand_mh].pConn)
	{
		mysqlSimpConnect(r->pool, af_db_cfg, &global_af_handler[rand_mh], &errBuf);
	}


	//query maste topics
	memset(tmp_str, 0, sizeof(tmp_str));
	snprintf(tmp_str, sizeof(tmp_str) - 1, "select id from af_topic_%s where ft = 1 order by %s desc limit %d, %d",
					  sbi.id, order_by_sql, offset, rec_per_page);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], tmp_str, &errBuf);

	
	//ap_rprintf(r, "<h1>%s, ret:%d</h2>", sql_str, ret);

	if(ret)
	{
		rec_num = mysqlNumRows(&global_af_handler[rand_mh]);
		
		if(rec_num > 0)
		{
			memset(rootTopicIdList, 0, sizeof(rootTopicIdList));
			
			while(global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]))
			{
				memset(tmp_str1, 0, sizeof(tmp_str1));
				if(i == 0)
				{
					sprintf(rootTopicIdList, "%d", atoi(global_af_handler[rand_mh].pRow[0]));
				}
				else
				{
					sprintf(tmp_str1, ",%d", atoi(global_af_handler[rand_mh].pRow[0]));
					strcat(rootTopicIdList, tmp_str1);
				}
				
				i++;
			}
			
			//free result
			mysqlFreeResult(&global_af_handler[rand_mh]);
		}
	}


	//query suit tree topic
	if(strlen(rootTopicIdList) <= 0)
	{
		//no suit tree topic
		
		//generl page list
		sprintf(opt_url, "%s?&s=2&p=", board_bname);
		general_page_list(r, &my_tpl, opt_url, 1, atoi(sbi.topics), rec_per_page, int_page);
		
		//get tpl content
		content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
		tpl_get_content(my_tpl, content);

		//output tpl content
		ap_rprintf(r, "%s", content);
		tpl_free(my_tpl);

		return;
	}

	//mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], "SET NAMES 'GB2312'", &errBuf);

	memset(tmp_str, 0, sizeof(tmp_str));
	sprintf(tmp_str, "select id, express, title, len, poster, posterId, rid, lay, \
					  hasAtt, postTime, hit, hot, locked, lastPost, attType from af_topic_%s where tt = 1 \
					  and rid in(%s) order by lastPost desc, \
					  ord asc", sbi.id, rootTopicIdList);

	//ap_rprintf(r, "<h1>%s</h2>", sql_str);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], tmp_str, &errBuf);

	if(ret)
	{
		rec_num = mysqlNumRows(&global_af_handler[rand_mh]);
		
		if(rec_num > 0)
		{	
			tpl_select_section(my_tpl, "treelist");

			while(global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]))
			{
				/*
				//模板公共变量处理
				forum_pub_varible_set(r, facfg, &my_tpl);
				tplSetVar(my_tpl, "show_board_id", board_id);
				tplSetVar(my_tpl, "show_cur_page", page);
				*/
				
				//topicId
				tpl_set_field_fmt(my_tpl, "show_topic_id", "%s", global_af_handler[rand_mh].pRow[0] ? global_af_handler[rand_mh].pRow[0] : "");
				
				//express
				extimg = global_af_handler[rand_mh].pRow[1];
				
				if(extimg && atoi(extimg) > 0)
				{
					//has express
					memset(tmp_str1, 0, sizeof(tmp_str1));
					sprintf(tmp_str1, "<img src=\"%s/images/smiles/%s.gif\" border=\"0\">", 
						facfg->appUrl , extimg);
					
					tpl_set_field_fmt(my_tpl, "show_express", "%s", tmp_str1);
				}
				else
				{
					tpl_set_field_fmt(my_tpl, "show_express", "%s", "");
				}
				
				//title
				tpl_set_field_fmt(my_tpl, "show_topic_title", "%s", global_af_handler[rand_mh].pRow[2] ? global_af_handler[rand_mh].pRow[2] : "");

				//topicLen
				tpl_set_field_fmt(my_tpl, "show_topic_len", "%s", global_af_handler[rand_mh].pRow[3] ? global_af_handler[rand_mh].pRow[3] : "0");
				
				//poster
				tpl_set_field_fmt(my_tpl, "show_poster", "%s", global_af_handler[rand_mh].pRow[4] ? global_af_handler[rand_mh].pRow[4] : "");
				
				//posterId
				tpl_set_field_fmt(my_tpl, "show_poster_id", "%s", global_af_handler[rand_mh].pRow[5] ? global_af_handler[rand_mh].pRow[5] : "");
				
				//rootTopicId
				tpl_set_field_fmt(my_tpl, "show_rootTopic_id", "%s", global_af_handler[rand_mh].pRow[6] ? global_af_handler[rand_mh].pRow[6] : "0");
				
				//space with, by lay
				lay = global_af_handler[rand_mh].pRow[7];
				memset(space_width, 0, sizeof(space_width));
				sprintf(space_width, "%d", lay ? atol(lay)*30 : 0);
				
				tpl_set_field_fmt(my_tpl, "show_space_width", "%s", space_width);
				 
				//li style			
				if(lay && atol(lay) == 0)
				{
					tpl_set_field_fmt(my_tpl, "li_style", "%s", "rectLi");
				}
				else if(lay && atol(lay) == 1)
				{
					tpl_set_field_fmt(my_tpl, "li_style", "%s", "circleLi");
				}
				else
				{
					tpl_set_field_fmt(my_tpl, "li_style", "%s", "squareLi");
				}
				
				//has attachment
				hasAtt = global_af_handler[rand_mh].pRow[8];
				if(hasAtt && atoi(hasAtt) == 1)
				{
					memset(tmp_str1, 0, sizeof(tmp_str1));
					sprintf(tmp_str1, "<img src=\"%s/images/icon_camera.gif\">", facfg->appUrl);
					
					tpl_set_field_fmt(my_tpl, "show_att_type", "%s", tmp_str1);
				}
				else
				{
					tpl_set_field_fmt(my_tpl, "show_att_type", "%s", "");
				}
				
				//post time
				postTime = global_af_handler[rand_mh].pRow[9];
				tpl_set_field_fmt(my_tpl, "show_post_time", "%s", postTime ? postTime : "");
				
				//hit number
				tpl_set_field_fmt(my_tpl, "show_topic_hits", "%s", 
								  global_af_handler[rand_mh].pRow[10] ? global_af_handler[rand_mh].pRow[10] : "0");

				
				//hot img
				memset(tmp_str, 0, sizeof(tmp_str));


				if(atoi(global_af_handler[rand_mh].pRow[0]) == atoi(global_af_handler[rand_mh].pRow[6]) && atoi(global_af_handler[rand_mh].pRow[11]) == 1)
				{
					snprintf(tmp_str, sizeof(tmp_str) - 1, "<img src=%s/images/hot.gif alt='Hot'>", facfg->appUrl);
				}

				tpl_set_field_fmt(my_tpl, "show_hot_img", "%s", tmp_str);


				//lock img
				memset(tmp_str, 0, sizeof(tmp_str));


				if(atoi(global_af_handler[rand_mh].pRow[0]) == atoi(global_af_handler[rand_mh].pRow[6]) && atoi(global_af_handler[rand_mh].pRow[12]) == 1)
				{
					snprintf(tmp_str, sizeof(tmp_str) - 1, "<img src=%s/images/lock.gif alt='Lock'>", facfg->appUrl);
				}

				tpl_set_field_fmt(my_tpl, "show_lock_img", "%s", tmp_str);


				//toped img
				memset(tmp_str, 0, sizeof(tmp_str));


				if(atoi(global_af_handler[rand_mh].pRow[0]) == atoi(global_af_handler[rand_mh].pRow[6]) && atol(global_af_handler[rand_mh].pRow[13]) == TOP_TOPIC_TIME_VALUE)
				{
					snprintf(tmp_str, sizeof(tmp_str) - 1, "<img src=%s/images/top.gif alt='Top'>", facfg->appUrl);
				}

				tpl_set_field_fmt(my_tpl, "show_top_img", "%s", tmp_str);

				
				//attach img
				memset(tmp_str, 0, sizeof(tmp_str));

				if(atoi(global_af_handler[rand_mh].pRow[14]) == 1)
				{
					snprintf(tmp_str, sizeof(tmp_str) - 1, "<img src=%s/images/att_img.gif>", facfg->appUrl);
				}

				tpl_set_field_fmt(my_tpl, "show_att_type", "%s", tmp_str);


				//////////
				tpl_append_section(my_tpl);
			}
			
			//free result
			mysqlFreeResult(&global_af_handler[rand_mh]);

			tpl_deselect_section(my_tpl);
		}
	}

	//ap_rprintf(r, "<h1>total_recs:%d</h1>", atoi(sbi.topics));
	
	//generl page list
	sprintf(opt_url, "%s?&s=2&p=", board_bname);
	general_page_list(r, &my_tpl, opt_url, 1, atoi(sbi.topics), rec_per_page, int_page);
	
	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);
}
