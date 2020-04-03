/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_sys_editboard.c
* file desc£ºsystem board edit and save
* author£ºAndyZhou<diudiu8848@163.com>
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


#include "fun_sys_editboard.h"

#define SYS_EDIT_BOARD_TPL "af_sys_editboard.html"
#define SYS_CAT_JS_TPL "af_sys_cat_js.html"
#define SYS_BOARD_JS_TPL "af_sys_board_js.html"

#define MODERATER_NUM 5

typedef struct catBoardInfoJs {
	int  cat_board_id;//board id
	int  parent_cat_id;//parent board id
	char title[MEDIM_MEM_SIZE];//title
	char bname[SMALL_MEM_SIZE];//mark
	int  show_type;//board show type 2:tree 1:thread
} catBoardInfoJs;

//moderater struct
typedef struct boardModerater {
	int uid;//user id
	char uname[SMALL_MEM_SIZE];//user nickname
} boardModerater;

//create new board tables
//1:ok 0:failed
int _sys_create_board_tables(request_rec* r, char* bname);

//create(update) board list js
void _sys_create_board_js(request_rec* r, af_app_config* facfg);

//get sub boards of signal cat
char *_sys_get_sub_board_list(request_rec *r, af_app_config* facfg, int cat_id,
							  struct catBoardInfoJs *suit_cat_board, int data_num);

void sys_edit_board(request_rec* r, af_app_config* facfg, 
						  apr_table_t* param_list)
{
	const char* bname = 0;
	const char* pid = 0;

	tpl_t *my_tpl;
	char* content = 0;
	char* dstIntro = 0;

	char tmp_str[SMALL_MEM_SIZE] = {0};
	char sql_str[BIG_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;
	int i = 0;

	boardModerater *bmoders;//moderater list
	int board_moders = 0;

	const char* brow_level = 0;
	const char* post_level = 0;
	const char* view_passwd = 0;
	const char* board_moderator = 0;
	const char* upload_type = 0;
	const char* board_bid = 0;


	bname = (char*)ap_table_get(param_list, "3");


	//show page header
	memset(tmp_str, 0, sizeof(tmp_str));
	if(bname && strcmp(bname, "(null)"))
	{
		sprintf(tmp_str, "Create Board");
	}
	else
	{
		sprintf(tmp_str, "Edit Board");
	}

	output_pageHeader(r, facfg, tmp_str,  ucocfg);

	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, SYS_TPL_DIR, SYS_EDIT_BOARD_TPL);

	//set general variable
	gen_pub_varible_set(r, facfg, &my_tpl);

	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	if(!global_af_handler[rand_mh].pConn)
	{
		mysqlSimpConnect(r->pool, af_db_cfg, &global_af_handler[rand_mh], &errBuf);
	}

	if(bname && strcmp(bname, "(null)"))
	{
		//get signal board info
		//show all cats list
		//mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], "SET NAMES 'GB2312'", &errBuf);
		memset(sql_str, 0, sizeof(sql_str));
		snprintf(sql_str, sizeof(sql_str) - 1,  "select id, title, bname, intro, isOpen, viewType, \
				 pid, browLevel, postLevel, passwd, moderator from af_boards \
				 where bname = '%s' limit 1", bname);
		
		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);
		
		if(ret)
		{
			rec_num = mysqlNumRows(&global_af_handler[rand_mh]);

			if(rec_num > 0)
			{
				global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]);

				board_bid = global_af_handler[rand_mh].pRow[0];

				tpl_set_field_fmt_global(my_tpl, "show_board_id", "%s", board_bid);
				tpl_set_field_fmt_global(my_tpl, "show_title", "%s", global_af_handler[rand_mh].pRow[1]);
				tpl_set_field_fmt_global(my_tpl, "show_bname", "%s", global_af_handler[rand_mh].pRow[2]);

				//convert board intro
				if(global_af_handler[rand_mh].pRow[3])
				{
					replaceStr(r, global_af_handler[rand_mh].pRow[3], &dstIntro, (char*)"<br>", (char*)"\n");
				}

				tpl_set_field_fmt_global(my_tpl, "show_intro", "%s", dstIntro);

				switch(atoi(global_af_handler[rand_mh].pRow[4]))
				{
				case 1:
					tpl_set_field_fmt_global(my_tpl, "isOpen_check_1", "%s", "checked");
					break;

				default:
					tpl_set_field_fmt_global(my_tpl, "isOpen_check_0", "%s", "checked");
					break;
				}

				
				switch(atoi(global_af_handler[rand_mh].pRow[5]))
				{
				case 2:
					tpl_set_field_fmt_global(my_tpl, "view_type_2", "%s", "checked");
					break;

				default:
					tpl_set_field_fmt_global(my_tpl, "view_type_1", "%s", "checked");
					break;

				}

				pid = global_af_handler[rand_mh].pRow[6];

				//free result
				//mysqlFreeResult(&global_af_handler[rand_mh]);

				//browse level
				brow_level = global_af_handler[rand_mh].pRow[7];
				
				memset(tmp_str, 0, sizeof(tmp_str));
				sprintf(tmp_str, "brow_level_%d", brow_level ? atoi(brow_level) : 0);
				tpl_set_field_fmt_global(my_tpl, tmp_str, "%s", "selected");

				//post level
				post_level = global_af_handler[rand_mh].pRow[8];
				memset(tmp_str, 0, sizeof(tmp_str));
				sprintf(tmp_str, "post_level_%d", post_level ? atoi(post_level) : 0);
				tpl_set_field_fmt_global(my_tpl, tmp_str, "%s", "selected");

				//view password
				view_passwd = global_af_handler[rand_mh].pRow[9];
				tpl_set_field_fmt_global(my_tpl, "show_pwd", "%s", view_passwd ? view_passwd : "");

				//board moderator
				board_moderator = global_af_handler[rand_mh].pRow[10];
				tpl_set_field_fmt_global(my_tpl, "show_moderator", "%s", board_moderator ? board_moderator : "");
			}
		}
	}


	//ap_rprintf(r, "<h1>pid:%s</h1>", pid);


	//show all cats list
	memset(sql_str, 0, sizeof(sql_str));
	snprintf(sql_str, sizeof(sql_str) - 1,  "select id, title, isOpen from af_boards where isOpen > 0 and pid = 0");
	
	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	if(ret)
	{
		rec_num = mysqlNumRows(&global_af_handler[rand_mh]);
		
		if(rec_num > 0)
		{
			//set sections list
			tpl_select_section(my_tpl, "catlists");

			while(global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]))
			{
				tpl_set_field_fmt(my_tpl, "cat_id", "%s", global_af_handler[rand_mh].pRow[0]);
				tpl_set_field_fmt(my_tpl, "cat_name", "%s", global_af_handler[rand_mh].pRow[1]);
				
				//ap_rprintf(r, "<h1>pid:%d, cid:%d</h1>", pid ? atoi(pid) : 0, atoi(global_af_handler[rand_mh].pRow[0]));

				if(pid && atoi(pid) == atoi(global_af_handler[rand_mh].pRow[0]))
				{
					tpl_set_field_fmt(my_tpl, "cur_cat_sel", "%s", "selected");
				}

				tpl_append_section(my_tpl);
			}

			tpl_deselect_section(my_tpl);

			//free result
			mysqlFreeResult(&global_af_handler[rand_mh]);
		}
	}


	//get board moderators
	if(bname && strcmp(bname, "(null)"))
	{
		memset(sql_str, 0, sizeof(sql_str));
		snprintf(sql_str, sizeof(sql_str) - 1,  "select uid, name from af_moderator where bname = '%s'", bname);
		
		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);
		
		if(ret)
		{
			rec_num = mysqlNumRows(&global_af_handler[rand_mh]);
			
			if(rec_num > 0)
			{
				//set sections list
				tpl_select_section(my_tpl, "moderlists");
				
				while(global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]))
				{
					tpl_set_field_fmt(my_tpl, "moder_uid", global_af_handler[rand_mh].pRow[0]);
					tpl_set_field_fmt(my_tpl, "moder_uname", "%s",  global_af_handler[rand_mh].pRow[1]);
					
					tpl_append_section(my_tpl);
				}
				
				tpl_deselect_section(my_tpl);
				
				//free result
				mysqlFreeResult(&global_af_handler[rand_mh]);
			}
		}
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



void sys_save_board(request_rec* r, af_app_config* facfg, 
						  ApacheRequest* req)
{
	char tmp_str[SMALL_MEM_SIZE] = {0};
	char sql_str[BIG_MEM_SIZE] = {0};
	char locationUrl[MEDIM_MEM_SIZE] = {0};
	char msgOk[SMALL_MEM_SIZE] = {0};
	char msgFail[SMALL_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	char moderator_list[MEDIM_MEM_SIZE] = {0};

	const char* bid = 0;
	const char* title = 0;
	const char* bname = 0;
	const char* pid = 0;
	const char* isOpen = 0;
	const char* viewType = 0;
	const char* browLevel = 0;
	const char* postLevel = 0;
	const char* passwd = 0;
	const char*	uploadType = 0;
	const char* intro = 0;

	////////////
	const char* tmp_title = 0;
	const char* tmp_bname = 0;
	int has_title = 0;
	int has_bname = 0;

	char* dstIntro = 0;

	//get input param 
	bid		= ApacheRequest_param(req, "bid");
	title	= ApacheRequest_param(req, "title");
	bname	= ApacheRequest_param(req, "bname");
	pid		= ApacheRequest_param(req, "pid");
	isOpen	= ApacheRequest_param(req, "isOpen");

	viewType	= ApacheRequest_param(req, "viewType");
	browLevel	= ApacheRequest_param(req, "browLevel");
	postLevel	= ApacheRequest_param(req, "postLevel");
	passwd		= ApacheRequest_param(req, "passwd");
	uploadType	= ApacheRequest_param(req, "uploadType");
	intro		= ApacheRequest_param(req, "intro");

	/*
	//get moderater param
	for(i = 0; i < MODERATER_NUM; i++)
	{
		//moder_name
		memset(tmp_str, 0, sizeof(tmp_str));
		sprintf(tmp_str, "moder_%d", i);
		sprintf(mp[i].mname, "%s", ApacheRequest_param(req, tmp_str));

		if(tmp_str && strcmp(tmp_str, "(null)"))
		{
			if(k == 0)
			{
				sprintf(moderator_list, "%s", tmp_str);
			}
			else
			{
				strcat(moderator_list, ",");
				strcat(moderator_list, tmp_str);
			}
			k++;
		}

		//moder_id
		memset(tmp_str, 0, sizeof(tmp_str));
		sprintf(tmp_str, "moderid_%d", i);
		sprintf(mp[i].mid, "%s", ApacheRequest_param(req, tmp_str));
	}
	*/

	sprintf(locationUrl, "%s/sys/editboard/%s", facfg->appRunUrl, bname ? bname : "");

	if(!title || !(*title))
	{
		//error param
		output_msgPage(r, facfg, ucocfg, "Need board name.", locationUrl, 0);
		return;
	}

	if(!bname || !(*bname))
	{
		//error param
		output_msgPage(r, facfg, ucocfg, "Need board ename", locationUrl, 0);
		return;
	}


	//convert bname to lower
	ap_str_tolower((char*)bname);

	if(!strcmp(bname, SYS_INTERNAL_RUN_NAME_1) || !strcmp(bname, SYS_INTERNAL_RUN_NAME_2)
	   || !strcmp(bname, SYS_INTERNAL_RUN_NAME_3))
	{
		//error param
		output_msgPage(r, facfg, ucocfg, "This board ename is limited.", locationUrl, 0);
		return;
	}

	if(!pid || !(*pid) || atoi(pid) <= 0)
	{
		//error param
		output_msgPage(r, facfg, ucocfg, "Need category", locationUrl, 0);
		return;
	}


	//ap_set_content_type(r, "text/html");
	
	//convert board intro
	replaceStr(r, intro, &dstIntro, (char*)"\n", (char*)"<br>");

	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	if(!global_af_handler[rand_mh].pConn)
	{
		mysqlSimpConnect(r->pool, af_db_cfg, &global_af_handler[rand_mh], &errBuf);
	}

	//check new board is empty
	if(!bid || atoi(bid) <= 0)
	{
		
		//mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], "SET NAMES 'GB2312'", &errBuf);

		memset(sql_str, 0, sizeof(sql_str));
		sprintf(sql_str, "select title, bname from af_boards where title = '%s' or bname = '%s'", title, bname);

		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

		if(!ret)
		{
			//query failed
			output_msgPage(r, facfg, ucocfg, "query failed, try again..", locationUrl, 0);
			return;
		}

		rec_num = mysqlNumRows(&global_af_handler[rand_mh]);

		if(rec_num > 0)
		{
			while(global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]))
			{
				tmp_title = global_af_handler[rand_mh].pRow[0];
				tmp_bname = global_af_handler[rand_mh].pRow[1];

				if(!strcmp(tmp_title, title))
				{
					has_title = 1;
					break;
				}

				if(!strcmp(tmp_bname, bname))
				{
					has_bname = 1;
					break;
				}
			}

			//free result
			mysqlFreeResult(&global_af_handler[rand_mh]);

			if(has_title)
			{
				output_msgPage(r, facfg, ucocfg, "board name is exist.", locationUrl, 0);
				return;
			}

			if(has_bname)
			{
				output_msgPage(r, facfg, ucocfg, "board ename is exist.", locationUrl, 0);
				return;
			}	
		}

		//create new board
		memset(sql_str, 0, sizeof(sql_str));
		sprintf(sql_str, "insert into af_boards(title, bname, pid, isOpen, browLevel, postLevel, passwd, \
						  viewType, uploadType, intro) values('%s', '%s', %d, %d, %d, %d, '%s', %d, \
						  '%s', '%s')", title, bname, atoi(pid), isOpen ? atoi(isOpen) : 0, atoi(browLevel),
						  atoi(postLevel), passwd ? passwd : "", atoi(viewType), uploadType ? uploadType : "",
						  dstIntro ? dstIntro : "");

		sprintf(msgOk, "%s", "create board success");
		sprintf(msgFail, "%s", "create board failed");
	}
	else
	{
		//update old board
		memset(sql_str, 0, sizeof(sql_str));
		sprintf(sql_str, "update af_boards set title = '%s', bname = '%s', isOpen = %d, browLevel = %d, \
						  postLevel = %d, passwd = '%s', viewType = %d, uploadType = '%s', intro = '%s', \
						  where id = %d", title, bname, isOpen ? atoi(isOpen) : 0, 
						  atoi(browLevel), atoi(postLevel), passwd ? passwd : "", atoi(viewType),
						  uploadType ? uploadType : "", dstIntro ? dstIntro : "", atoi(bid));

		sprintf(msgOk, "%s", "update board success");
		sprintf(msgFail, "%s", "update board failed");
	}


	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	if(!ret)
	{
		//query failed
		output_msgPage(r, facfg, ucocfg, errBuf, locationUrl, 0);
		return;
	}

	if(!bid || atoi(bid) <= 0)
	{
		//if new board, create new board tables
		ret = _sys_create_board_tables(r, (char*)bname);
	}

	//create board list js
	_sys_create_board_js(r, facfg);
	
	output_msgPage(r, facfg, ucocfg, msgOk, locationUrl, 1);

	return;
}


//create(update) board list js
void _sys_create_board_js(request_rec* r, af_app_config* facfg)
{
	tpl_t *my_tpl;
	struct catBoardInfoJs *suit_cat_board;

	char tmp_str[MEDIM_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	const char *result = NULL;
	const char *tmp_result = NULL;
	char* content = 0;

	apr_status_t rv;
	apr_file_t *fp = NULL;
	apr_size_t blen = 0;

	int i = 0;
	int m = 0;
	int n = 0;
	int total_subject_num = 0;
	int total_post_num = 0;

	char total_subject_str[TINY_MEM_SIZE] = {0};
	char total_post_str[TINY_MEM_SIZE] = {0};

	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, SYS_TPL_DIR, SYS_CAT_JS_TPL);

	//set general variable
	gen_pub_varible_set(r, facfg, &my_tpl);

	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	if(!global_af_handler[rand_mh].pConn)
	{
		mysqlSimpConnect(r->pool, af_db_cfg, &global_af_handler[rand_mh], &errBuf);
	}

	//get all cats and boards
	//mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], "SET NAMES 'GB2312'", &errBuf);
	memset(tmp_str, 0, sizeof(tmp_str));
	snprintf(tmp_str, sizeof(tmp_str) - 1,  "select id, title, bname, pid, \
				viewType from af_boards where isOpen = 1 order by fcat, sortNum");
	
	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], tmp_str, &errBuf);
	
	if(ret)
	{
		rec_num = mysqlNumRows(&global_af_handler[rand_mh]);
		
		if(rec_num > 0)
		{
			
			//define dynamic struct array
			suit_cat_board = (struct catBoardInfoJs *)apr_palloc(r->pool, rec_num * sizeof(struct catBoardInfoJs));
			suit_cat_board = (struct catBoardInfoJs *)apr_pcalloc(r->pool, rec_num * sizeof(struct catBoardInfoJs));

			while(global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]))
			{
				suit_cat_board[i].cat_board_id = global_af_handler[rand_mh].pRow[0] ? atoi(global_af_handler[rand_mh].pRow[0]) : 0;//board id
				sprintf(suit_cat_board[i].title, "%s", global_af_handler[rand_mh].pRow[1] ? global_af_handler[rand_mh].pRow[1] : "");//title
				sprintf(suit_cat_board[i].bname, "%s", global_af_handler[rand_mh].pRow[2] ? global_af_handler[rand_mh].pRow[2] : "");//bname
				
				suit_cat_board[i].parent_cat_id = global_af_handler[rand_mh].pRow[3] ? atoi(global_af_handler[rand_mh].pRow[3]) : 0;//pid
				suit_cat_board[i].show_type = global_af_handler[rand_mh].pRow[4] ? atoi(global_af_handler[rand_mh].pRow[4]) : 1;

				i++;
			}

			//free result
			mysqlFreeResult(&global_af_handler[rand_mh]);
		}
	}


	//set sections list
	tpl_select_section(my_tpl, "catboardJslist");

	for(m = 0; m < rec_num; m++)
	{
		//list cats
		if(suit_cat_board[m].parent_cat_id == 0)
		{
			//list suit sub boards
			tmp_result = _sys_get_sub_board_list(r, facfg, suit_cat_board[m].cat_board_id, suit_cat_board, rec_num);
			tpl_set_field_fmt(my_tpl, "show_board_sel_js_list", "%s", tmp_result);
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

	tpl_free(my_tpl);

	//write content to js file
	memset(tmp_str, 0, sizeof(tmp_str));
	snprintf(tmp_str, sizeof(tmp_str) - 1, "%s/af_boards.js", facfg->appUploadPath);

	r->filename = ap_psprintf(r->pool, tmp_str);

	rv = apr_file_open(&fp, r->filename, APR_CREATE|APR_WRITE|APR_TRUNCATE, APR_OS_DEFAULT, r->pool);

	if(rv != APR_SUCCESS)
	{
		return;
	}

	blen = strlen(content);
	rv = apr_file_write(fp, content, &blen);

	apr_file_close(fp);
}


//get sub boards of signal cat
char *_sys_get_sub_board_list(request_rec *r, af_app_config* facfg, int cat_id,
							  struct catBoardInfoJs *suit_cat_board, int data_num)
{
	tpl_t *_my_tpl;
	char* content = 0;

	const char *return_tpl_content = NULL;
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
	tpl_file_init(r, facfg, &_my_tpl, SYS_TPL_DIR, SYS_BOARD_JS_TPL);

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

			sprintf(tmp_str, "%d", suit_cat_board[i].cat_board_id);
			sprintf(tmp_str_1, "%d", cat_id);


			tpl_set_field_fmt(_my_tpl, "show_board_bname", "%s", suit_cat_board[i].bname ? suit_cat_board[i].bname : tmp_str);

			tpl_set_field_fmt(_my_tpl, "show_board_name", "%s", suit_cat_board[i].title);
			
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







//create new board tables
//1:ok 0:failed
int _sys_create_board_tables(request_rec* r, char* bname)
{
	char sql_str[BIG_MEM_SIZE * 2] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	const char* bid = 0;


	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	//check new board id by bname
	memset(sql_str, 0, sizeof(sql_str));
	sprintf(sql_str, "select id from af_boards where bname = '%s' limit 1", bname);
	
	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	if(!ret)
	{
		//query failed
		return 0;
	}
	
	rec_num = mysqlNumRows(&global_af_handler[rand_mh]);

	if(rec_num <= 0)
	{
		return 0;
	}
	
	global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]);
	bid = global_af_handler[rand_mh].pRow[0];
	
	//new tables
	memset(sql_str, 0, sizeof(sql_str));
	sprintf(sql_str, "create table af_topic_%d ( \
	id mediumint(8) unsigned not null auto_increment, \
	title char(128) binary default null default \'\', \
	express tinyint(2) unsigned not null default 0, \
	reply smallint(5) unsigned not null default 0, \
	hit mediumint(8) unsigned not null default 0, \
	hasAtt tinyint(2) unsigned not null default 0, \
	attType tinyint(2) unsigned not null default 0, \
	len mediumint(8) unsigned not null default 0, \
	hot tinyint(2) unsigned not null default 0, \
	locked tinyint(2) unsigned not null default 0, \
	pid mediumint(8) unsigned not null default 0, \
	rid mediumint(8) unsigned not null default 0, \
	lay tinyint(3) unsigned not null default 0, \
	ord smallint(5) unsigned not null default 0, \
	poster char(32) binary not null default \'\', \
	posterId mediumint(8) unsigned not null default 0, \
	lastPoster char(32) binary not null default \'\', \
	lastPosterId mediumint(8) unsigned not null default 0, \
	ft tinyint(2) unsigned not null default 0, \
	tt tinyint(2) unsigned not null default 0, \
	postTime char(20) binary not null default \'\', \
	lastPost int(10) unsigned not null default 0, \
	index tsort(tt, rid, lastPost, ord), \
	index fsort(ft, lastPost), \
	index fhsort(ft, hit), \
	index frsort(ft, reply), \
	index tlast(lastPost), \
	primary key(id) \
	) TYPE=MyISAM PACK_KEYS=1", atoi(bid));

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	if(!ret)
	{
		return 0;
	}

	//new tables
	memset(sql_str, 0, sizeof(sql_str));
	sprintf(sql_str, "create table af_content_%d ( \
	id mediumint(8) unsigned not null default 0, \
	title varchar(128) binary default null default \'\', \
	express tinyint(2) unsigned not null default 0, \
	rid mediumint(8) unsigned not null default 0, \
	hasAtt tinyint(2) unsigned not null default 0, \
	attType tinyint(2) unsigned not null default 0, \
	attList varchar(255) binary not null default \'\', \
	hot tinyint(2) unsigned not null default 0, \
	locked tinyint(2) unsigned not null default 0, \
	content text, \
	posterId mediumint(8) unsigned not null default 0, \
	poster varchar(32) binary not null default '', \
	postTime varchar(20) binary not null default \'\', \
	index rid(rid), \
	index posterId(posterId), \
	index title(title), \
	primary key(id) \
	)TYPE=MyISAM PACK_KEYS=1", atoi(bid));

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	if(!ret)
	{
		return 0;
	}

	return 1;
}

