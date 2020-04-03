/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name：fun_forum_post.c
* file desc：forum post or reply topic
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
#include "fun_forum_post.h"

//post topic tpl file
#define FORUM_POST_TOPIC_TPL "af_post.html"


//save topic 
void save_post_data(request_rec *r, char *query_post_data);

//update topic and post number, and last poster in signal board
//topicType:1 reply topic default:maste topic
void updateTopicNum(char *cat_id, char *board_id);


//post topic form
void topic_post(request_rec *r, af_app_config* facfg,
				const char* board_bname, signal_board_info sbi,
				apr_table_t* param_list, int isEdit)
{	
	tpl_t *my_tpl;
	char* content = 0;

	char tmp_str[SMALL_MEM_SIZE] = {0};
	char sql_str[MEDIM_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	const char* topic_id = 0;//topic id will be reply
	int int_topic_id = 0;

	const char* topic_type = 0;//topic show type
	int int_topic_type = 0;
	
	const char* cur_page = 0;
	char location_url[MEDIM_MEM_SIZE] = {0};

	//for edit
	const char* old_topic_id = 0;
	const char* old_title = 0;
	const char* old_express = 0;
	const char* old_posterId = 0;
	const char* old_content = 0;
	const char* old_locked = 0;
	const char* old_attList = 0;

	//for reply
	char* rep_title = 0;
	char* rep_content = 0;
	char* rep_content_two = 0;
	int size = 0;
	int big_content = 0;
	char* dstContent = 0;

	//check current user is logined
	if(!ucocfg.available)
	{
		//internal redirect
		sprintf(location_url, "/%s/union/login/:%s?cmd=post", facfg->appRunUrl, board_bname);
		ap_internal_redirect(location_url, r);
		return;
	}

	topic_id	= ap_table_get(param_list, "t");
	topic_type	= ap_table_get(param_list, "s");
	cur_page	= ap_table_get(param_list, "p");

	if(!global_af_handler[rand_mh].pConn)
	{
		mysqlSimpConnect(r->pool, af_db_cfg, &global_af_handler[rand_mh], &errBuf);
	}


	//get old topic info for edit or reply
	if(topic_id && atoi(topic_id) > 0)
	{
		memset(location_url, 0, sizeof(location_url));
		sprintf(location_url, "%s/%s?s=%s", facfg->appRunUrl, board_bname, topic_type);

		//get rand mysql handler
		rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

		//get suit topic content
		//mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], "SET NAMES 'GB2312'", &errBuf);
		memset(sql_str, 0, sizeof(sql_str));
		snprintf(sql_str, sizeof(sql_str) - 1,  "select id, title,  express,  posterId, content, locked, attList from af_content_%s \
						 where id = %d limit 1", sbi.id,  atoi(topic_id));
	
		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);
	
		if(!ret)
		{
			output_msgPage(r, facfg, ucocfg, "Invalid topic info", location_url, 0);
			return;
		}

		rec_num = mysqlNumRows(&global_af_handler[rand_mh]);
		
		if(rec_num <= 0)
		{
			output_msgPage(r, facfg, ucocfg, "Invalid topic info", location_url, 0);
			return;
		}

		global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]);

		old_topic_id 	= global_af_handler[rand_mh].pRow[0] ;
		old_title 		= global_af_handler[rand_mh].pRow[1] ;
		old_express 	= global_af_handler[rand_mh].pRow[2] ;
		old_posterId 	= global_af_handler[rand_mh].pRow[3] ;
		old_content 	= global_af_handler[rand_mh].pRow[4] ;
		old_locked		= global_af_handler[rand_mh].pRow[5] ;
		old_attList		= global_af_handler[rand_mh].pRow[6] ;


		if(old_locked && atoi(old_locked) == 1)
		{
			output_msgPage(r, facfg, ucocfg, "Topic is locked", location_url, 0);
			return;
		}

		if(isEdit && old_posterId &&  atoi(old_posterId) !=  ucocfg.uid)
		{
			output_msgPage(r, facfg, ucocfg, "No access to edit the topic.", location_url, 0);
			return;
		}
	}

	//show page header
	output_pageHeader(r, facfg, sbi.name,  ucocfg);

	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, FORUM_TPL_DIR, FORUM_POST_TOPIC_TPL);

	//set general variable
	gen_pub_varible_set(r, facfg, &my_tpl);
	
	tpl_set_field_fmt_global(my_tpl, "show_board_bname", "%s", board_bname ? board_bname : sbi.id);
	tpl_set_field_fmt_global(my_tpl, "show_board_name", "%s", sbi.name ? sbi.name : "");
	tpl_set_field_fmt_global(my_tpl, "show_topic_id", "%s", topic_id ? topic_id : "");
	tpl_set_field_fmt_global(my_tpl, "show_board_type", "%s", topic_type ? topic_type : "1");
	tpl_set_field_fmt_global(my_tpl, "show_cur_page", "%s", cur_page ? cur_page : "1");

	if(topic_id && atoi(topic_id) > 0)
	{
		if(isEdit)
		{
			//edit signal topic
			tpl_set_field_fmt_global(my_tpl, "post_desc", "%s", "Edit");
			tpl_set_field_fmt_global(my_tpl, "show_cmd", "%s", "uptopic");	


			//express
			memset(tmp_str, 0, sizeof(tmp_str));
			sprintf(tmp_str, "exp_check_%d", atoi(old_express));
			
			//title
			tpl_set_field_fmt_global(my_tpl, "show_old_title", "%s",  old_title);	

			//content
			replaceStr(r, old_content, &dstContent, (char*)"\"", (char*)"\\\"");
			replaceStr(r, dstContent, &dstContent, (char*)"\r", (char*)"<br>");

			tpl_set_field_fmt_global(my_tpl, "show_old_content", "%s", dstContent);	

			//current express
			tpl_set_field_fmt_global(my_tpl, tmp_str, "%s", "checked");
			tpl_set_field_fmt_global(my_tpl, "run_init_topic", "%s", "setTopicOldContent();");

			
			tpl_set_field_fmt_global(my_tpl, "show_imgurl", "%s", old_attList ? old_attList : "");

		}
		else
		{
			//reply signal topic
			tpl_set_field_fmt_global(my_tpl, "post_desc", "%s", "Reply");
			tpl_set_field_fmt_global(my_tpl, "show_cmd", "%s", "savetopic");	

			//format topic and content for reply
			size = strlen(old_title);
			rep_title = (char*)apr_palloc(r->pool, sizeof(char) * (size + 10));
			rep_title = ap_psprintf(r->pool, "re:%s", old_title);

			size = strlen(old_content);

			if(size > SMALL_MEM_SIZE)
			{
				//content(conver)
				replaceStr(r, old_content, &dstContent, (char*)"\"", (char*)"\\\"");

				//get sub string of content, and add .... for more data
				getSubStr(r, &rep_content, dstContent, 0,  SMALL_MEM_SIZE);
				big_content = 1;
			}
			else
			{
				replaceStr(r, old_content, &dstContent, (char*)"\"", (char*)"\\\"");
				rep_content = (char*)apr_palloc(r->pool, sizeof(char) * (size + 20));
				rep_content = ap_psprintf(r->pool, "%s", dstContent);
			}

			size = strlen(rep_content);

			rep_content_two = (char*)apr_palloc(r->pool, sizeof(char) * (size + SMALL_MEM_SIZE));
			rep_content_two = ap_psprintf(r->pool, "-----------------<br>>> %s %s<br>-----------------<br><br>", rep_content, big_content ? "。。。" : "");
			
			tpl_set_field_fmt_global(my_tpl, "show_old_title", "%s",  rep_title);	
			tpl_set_field_fmt_global(my_tpl, "show_old_content", "%s", rep_content_two);	
			tpl_set_field_fmt_global(my_tpl, "run_init_topic", "%s", "setTopicOldContent();");
		}
	}
	else
	{
		//post new topic
		tpl_set_field_fmt_global(my_tpl, "post_desc", "%s", "Post");
		tpl_set_field_fmt_global(my_tpl, "show_cmd", "%s", "savetopic");
	}
	
	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);

	
	//ouput page bottom
	output_pageBottom(r, facfg);
}


//topic save
void topic_save(request_rec *r, ApacheRequest* req, af_app_config* facfg, int isUpdate)
{
	
	const char *board_show_type = 0;
	const char *post_job = 0;
	const char *board_bname = 0;
	const char *page = 0;

	const char *topic_id = 0;
	const char *title = 0;
	const char *express = 0;
	const char *imgurl = 0;
	const char *content = 0;
	char* dstContent = 0;


	int int_topic_id = 0;

	int sql_len = 0;
	const char *sql_str = 0;
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	int hasAtt = 0;//has attach?
	int attType = 0;//attach type

	char tmp_url[SMALL_MEM_SIZE] = {0};
	char location_url[MEDIM_MEM_SIZE] = {0};

	char newTopicId[TINY_MEM_SIZE] = {0};//new topic id
	char rootTopicId[TINY_MEM_SIZE] = {0};//root topic id
	char fatherTopicLayer[TINY_MEM_SIZE] = {0};//father layer value
	char fatherTopicOrders[TINY_MEM_SIZE] = {0};//father topic orders
	char newTopicLayer[TINY_MEM_SIZE] = {0};//new topic layer
	char newTopicOrders[TINY_MEM_SIZE] = {0};//new topic orders

	//get current time
	char intPostTime[TINY_MEM_SIZE] = {0}; //post topic time
	char postTimeStr[TINY_MEM_SIZE] = {0};
	char cur_day_str[TINY_MEM_SIZE] = {0};

	signal_board_info sbi = {0};//signal board info

	struct tm *now_time;
	time_t nowtime;
	const char* old_root_topic_id = 0;
	const char* topic_is_locked = 0;


	//check current user is logined
	if(!ucocfg.available)
	{
		//internal redirect
		sprintf(location_url, "/%s/union/login/:%s?cmd=post", facfg->appRunUrl, board_bname);
		ap_internal_redirect(location_url, r);
		return;
	}

	
	//get board bname from POST param
	board_bname	= ApacheRequest_param(req, "bname");

	//ap_set_content_type(r, "text/html");
	//ap_rprintf(r, "<h1>board_bname:%s</h1>", board_bname);

	if(!board_bname || strlen(board_bname) <= 0)
	{
		//invalid board
		output_msgPage(r, facfg, ucocfg, "Need board info", facfg->appRunUrl, 0);
		return;
	}

	//get signal board info
	ret = get_signal_board_info(r, (char*)board_bname, &sbi);

	if(!ret)
	{
		//invalid board
		output_msgPage(r, facfg, ucocfg, "Invalid board.", facfg->appRunUrl, 0);
		return;
	}

	nowtime = time(NULL);
	now_time = localtime(&nowtime);

	strftime(postTimeStr, 20, "%Y/%m/%d %H:%M", now_time);
	strftime(cur_day_str, 10, "%y%m%d", now_time);
	sprintf(intPostTime, "%ld", nowtime);	

	board_show_type = ApacheRequest_param(req, "type");
	page			= ApacheRequest_param(req, "page");

	topic_id		= ApacheRequest_param(req, "topicId");
	title			= ApacheRequest_param(req, "title");
	express			= ApacheRequest_param(req, "express");
	content			= ApacheRequest_param(req, "content");
	imgurl			= ApacheRequest_param(req, "imgurl");

	if(!title || strlen(title) <= 0)
	{
		//invalid title
		sprintf(location_url, "%s/%s?cmd=post&t=%s&s=%s", facfg->appRunUrl, board_bname,
							  topic_id ? topic_id : "", board_show_type ? board_show_type : "");
		output_msgPage(r, facfg, ucocfg, "Need topic", facfg->appRunUrl, 0);
		return;
	}

	if(!content || strlen(content) <= 0)
	{
		sprintf(location_url, "%s/%s?cmd=post&t=%s&s=%s", facfg->appRunUrl, board_bname,
							  topic_id ? topic_id : "", board_show_type ? board_show_type : "");
		output_msgPage(r, facfg, ucocfg, "Need topic content", facfg->appRunUrl, 0);
		return;
	}

	//init memory for sql string
	sql_len = strlen(content) + BIG_MEM_SIZE;	
	sql_str = (char *)apr_palloc(r->pool, sql_len);
	sql_str = (char *)apr_pcalloc(r->pool, sql_len);

	//convert topic content
	replaceStr(r, content, &dstContent, (char*)"<br>", (char*)"");
	//replaceStr(r, dstContent, &dstContent, (char*)"<", (char*)"&lt;");
	//replaceStr(r, dstContent, &dstContent, (char*)">", (char*)"&gt;");
	replaceStr(r, dstContent, &dstContent, (char*)"\n", (char*)"<br>");
	replaceStr(r, dstContent, &dstContent, (char*)"\'", (char*)"\\'");

	replaceStr(r, dstContent, &dstContent, (char*)"<script", (char*)"&lt;script");
	replaceStr(r, dstContent, &dstContent, (char*)"<css", (char*)"&lt;css");
	replaceStr(r, dstContent, &dstContent, (char*)"<img", (char*)"&lt;img");
	replaceStr(r, dstContent, &dstContent, (char*)"<marque", (char*)"&lt;marque");

	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	if(!global_af_handler[rand_mh].pConn)
	{
		mysqlSimpConnect(r->pool, af_db_cfg, &global_af_handler[rand_mh], &errBuf);
	}
	
	//mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], "SET NAMES 'GB2312'", &errBuf);


	//check topic owner is ok if update signal topic
	if(isUpdate && topic_id)
	{
		memset(location_url, 0, sizeof(location_url));
		sprintf(location_url, "%s/%s?s=%s", facfg->appRunUrl, board_bname, board_show_type);


		//get suit topic content
		sql_str = (char *)apr_pcalloc(r->pool, sql_len);
		sql_str = ap_psprintf(r->pool, "select  posterId, rid from af_content_%s where id = %d limit 1", sbi.id,  atoi(topic_id));
	
		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);
	
		if(!ret)
		{
			output_msgPage(r, facfg, ucocfg, "Invlid topic info", location_url, 0);
			return;
		}

		rec_num = mysqlNumRows(&global_af_handler[rand_mh]);
		
		if(rec_num <= 0)
		{
			output_msgPage(r, facfg, ucocfg, "Invlid topic info", location_url, 0);
			return;
		}

		global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]);

		if(global_af_handler[rand_mh].pRow[0] &&  atoi(global_af_handler[rand_mh].pRow[0] ) !=  ucocfg.uid)
		{
			output_msgPage(r, facfg, ucocfg, "No right for update topic", location_url, 0);
			return;
		}

		old_root_topic_id = global_af_handler[rand_mh].pRow[1];
	}


	//only update topic and content
	if(isUpdate && topic_id)
	{
		//set reload url
		memset(location_url, 0, sizeof(location_url));
		sprintf(location_url, "%s/%s?cmd=read&i=%d&t=%d&s=%d", facfg->appRunUrl, board_bname, atoi(topic_id), atoi(old_root_topic_id), atoi(board_show_type));

		//update title, express, len, attType
		sql_str = (char *)apr_pcalloc(r->pool, sql_len);
		sql_str = ap_psprintf(r->pool, "update af_topic_%s set title = '%s', express = %d,  len = %d, attType = %d where id = %d",
			sbi.id, title, atoi(express), strlen(content), imgurl ? 1 : 0, atoi(topic_id));

		
		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);
		
		if(!ret)
		{
			//update failed
			output_msgPage(r, facfg, ucocfg, "update topic failed.", location_url, 0);
			return;
		}

		//update content and topic for content table
		sql_str = (char *)apr_pcalloc(r->pool, sql_len);
		sql_str = ap_psprintf(r->pool, "update af_content_%s set title = '%s', express = %d,  content = '%s', \
										attType = %d, attList = '%s' where id = %d",
										sbi.id, title, atoi(express), dstContent, imgurl ? 1 : 0, imgurl, atoi(topic_id));

		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);
		
		if(!ret)
		{
			//insert failed
			output_msgPage(r, facfg, ucocfg, "update topic content failed.", location_url, 0);
			return;
		}
		
		//page relocation
		output_msgPage(r, facfg, ucocfg, "update topic success", location_url, 1);

		return;
	}




	//set reload url
	memset(location_url, 0, sizeof(location_url));
	sprintf(location_url, "%s/%s?s=%d", facfg->appRunUrl, board_bname, atoi(board_show_type));
	
	/*
	r->content_type = "text/html";
	ap_send_http_header(r);
	ap_rprintf(r, "<h1>dstContent:%s</h1>", dstContent);
	return;
	*/

	//get rand mysql handler
	//rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	int_topic_id = topic_id ? atoi(topic_id) : 0;	

	//if father topic id is not empty, get father info by query db
	if(int_topic_id > 0)
	{
		//get root topic id, father layer and orders value
		sql_str = ap_psprintf(r->pool, "select rid, lay, ord, locked from af_topic_%s where id = %d limit 1",
										sbi.id, int_topic_id);
		
		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

		if(!ret)
		{
			//query failed
			output_msgPage(r, facfg, ucocfg, "Query topic failed", location_url, 0);
			return;
		}
		
		rec_num = mysqlNumRows(&global_af_handler[rand_mh]);
		
		if(rec_num > 0)
		{
			
			global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]);

			//if topic is locked
			topic_is_locked = global_af_handler[rand_mh].pRow[3];

			if(topic_is_locked && atoi(topic_is_locked) == 1)
			{
				output_msgPage(r, facfg, ucocfg, "topic is locked.", location_url, 0);
				return;
			}
			
			//root topic id
			sprintf(rootTopicId, "%s", global_af_handler[rand_mh].pRow[0] ? global_af_handler[rand_mh].pRow[0] : "0");
			
			//father record layer value
			sprintf(fatherTopicLayer, "%s", global_af_handler[rand_mh].pRow[1] ? global_af_handler[rand_mh].pRow[1] : "0");
			
			//father record order value
			sprintf(fatherTopicOrders, "%s", global_af_handler[rand_mh].pRow[2] ? global_af_handler[rand_mh].pRow[2] : "0");

			
			//更新插入位置后记录的orders值
			//更新相应帖子orders值

			//update orders value after insert record and update order value of suit topics
			sql_str = (char *)apr_pcalloc(r->pool, sql_len);
			sql_str = ap_psprintf(r->pool, "update af_topic_%s set ord = ord+1 where rid = %d and ord > %d",
											sbi.id, atoi(rootTopicId), atol(fatherTopicOrders));
			
			ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

			if(!ret)
			{
				//update failed
				output_msgPage(r, facfg, ucocfg, "update relate topic failed.", location_url, 0);
				return;
			}
		}
	}

	if(int_topic_id <= 0)
	{	
		
		//new topic, suit two method
		sql_str = ap_psprintf(r->pool, "insert into af_topic_%s(express,title, len, tt, ft, \
										poster, posterId, postTime, lastPost, lastPoster, lastPosterId, attType) values('%s','%s', %d, 1, 1, \
										'%s', %d, '%s', %ld, '%s', %d, %d)", sbi.id, express, title, strlen(content),
										ucocfg.nick, ucocfg.uid, postTimeStr, atol(intPostTime), ucocfg.nick, ucocfg.uid,
										strlen(imgurl) >= 10 ? 1 : 0); 

		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);
		
		if(!ret)
		{
			//insert failed
			output_msgPage(r, facfg, ucocfg, "post topic failed.", location_url, 0);
			return;
		}

		//update rid value of new topic
		sql_str = (char *)apr_pcalloc(r->pool, sql_len);
		sql_str = ap_psprintf(r->pool, "update af_topic_%s set rid = id where id = last_insert_id()", sbi.id);

		
		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);
		
		if(!ret)
		{
			//update failed
			output_msgPage(r, facfg, ucocfg, "update topic failed.", location_url, 0);
			return;
		}

		//save content of new topic
		sql_str = (char *)apr_pcalloc(r->pool, sql_len);
		sql_str = ap_psprintf(r->pool, "insert into af_content_%s(id, title, express, rid, content, postTime,  posterId, poster, attType, attList) \
										values(last_insert_id(), '%s', '%s', last_insert_id(), '%s', '%s', %d, '%s', %d, '%s')", 
										sbi.id, title, express, dstContent, postTimeStr, ucocfg.uid, ucocfg.nick, strlen(imgurl) >= 10 ? 1 : 0, imgurl);

		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);
		
		if(!ret)
		{
			//insert failed
			output_msgPage(r, facfg, ucocfg, "save topic content failed.", location_url, 0);
			//output_msgPage(r, facfg, ucocfg, errBuf, location_url, 0);
			return;
		}

		//update topics, posts, laste poster of signal board

		//last poster
		memset(tmp_url, 0, sizeof(tmp_url));
		snprintf(tmp_url, sizeof(tmp_url) - 1, "%s - <a href=%s/union/%d>%s</a>", postTimeStr, facfg->appRunUrl,
				 ucocfg.uid, ucocfg.nick);

		sql_str = (char *)ap_pcalloc(r->pool, sql_len);
		sql_str = ap_psprintf(r->pool, "update af_boards set topics = topics + 1, posts = posts + 1, \
										lastPost = '%s', lastUpdate = %d where id = %d",
										tmp_url, atoi(cur_day_str), atoi(sbi.id));
		
		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);
		
	}
	else
	{
		
		//reply topic
		sql_str = (char *)ap_pcalloc(r->pool, sql_len);
		sql_str = ap_psprintf(r->pool, "insert into af_topic_%s(rid, pid, express, title, len, \
										poster, posterId, lay, ord, tt, postTime, lastPost, attType) \
										values('%s', %d, '%s','%s', %d,'%s', %d, %d, %d, 1, '%s', %ld, %d)",
										sbi.id, rootTopicId, atoi(topic_id), express, title, strlen(content),
										ucocfg.nick, ucocfg.uid, atoi(fatherTopicLayer) + 1, atoi(fatherTopicOrders) + 1,
										postTimeStr, atol(intPostTime), (imgurl && strlen(imgurl) >= 10) ? 1 : 0); 

		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);
		
		if(!ret)
		{
			//insert failed
			output_msgPage(r, facfg, ucocfg, "reply topic failed.", location_url, 0);
			return;
		}

		//save content of reply topic
		sql_str = (char *)ap_pcalloc(r->pool, sql_len);
		sql_str = ap_psprintf(r->pool, "insert into af_content_%s(id, title, express, rid, content, posterId, poster, postTime, attType, attList) \
										values(last_insert_id(), '%s', '%s', %d, '%s', %d, '%s', '%s', %d, '%s')", 
										sbi.id, title, express, atoi(rootTopicId), dstContent, ucocfg.uid, ucocfg.nick, postTimeStr,
										(imgurl && strlen(imgurl) >= 10) ? 1 : 0, imgurl);

		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);
		
		if(!ret)
		{
			//insert failed
			output_msgPage(r, facfg, ucocfg, "save topic content failed.", location_url, 0);
			//output_msgPage(r, facfg, ucocfg, errBuf, location_url, 0);
			return;
		}

		//update laste post time of root topic
		sql_str = (char *)ap_pcalloc(r->pool, sql_len);
		sql_str = ap_psprintf(r->pool, "update af_topic_%s set lastPost= %ld where rid = %d",
										sbi.id, atol(intPostTime), atoi(rootTopicId));
		
		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);
		

		//update last post time, last poster, reply number of master topic
		sql_str = (char *)ap_pcalloc(r->pool, sql_len);
		sql_str = ap_psprintf(r->pool, "update af_topic_%s set reply = reply + 1, lastPoster = '%s', lastPosterId = %d, \
										postTime = '%s' where id = %d", sbi.id, ucocfg.nick,
										ucocfg.uid, postTimeStr, atoi(rootTopicId));
		
		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

		//update posts, last poster and time of signal board
		memset(tmp_url, 0, sizeof(tmp_url));
		snprintf(tmp_url, sizeof(tmp_url) - 1, "%s - <a href=%s/union/%d>%s</a>", postTimeStr, facfg->appRunUrl,
				 ucocfg.uid, ucocfg.nick);

		sql_str = (char *)ap_pcalloc(r->pool, sql_len);
		sql_str = ap_psprintf(r->pool, "update af_boards set posts = posts + 1, lastPost = '%s', \
										lastUpdate = %d where id = %d",
										tmp_url, atoi(cur_day_str), atoi(sbi.id));
		
		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

		//update user posts number
		sql_str = (char *)ap_pcalloc(r->pool, sql_len);
		sql_str = ap_psprintf(r->pool, "update af_member set posts = posts + 1 where id = %d", ucocfg.uid);
		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);
		
		
	}

	//page relocation
	output_msgPage(r, facfg, ucocfg, "topic post success.", location_url, 1);
}
