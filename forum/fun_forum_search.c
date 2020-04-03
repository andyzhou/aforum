/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_forum_search.c
* file desc£ºforum topic search funcs
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
#include "fun_forum_search.h"

#define FORUM_SEARCH_TPL "af_search.html"
#define FORUM_SEARCH_RESULT_TPL "af_search_result.html"

//show search form
void topic_search_form(request_rec* r, af_app_config* facfg)
{
	tpl_t *my_tpl;
	char* content = 0;

	//show page header
	output_pageHeader(r, facfg, "Search",  ucocfg);

	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, FORUM_TPL_DIR, FORUM_SEARCH_TPL);

	//set general variable
	gen_pub_varible_set(r, facfg, &my_tpl);
	
	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);

	
	//ouput page bottom
	output_pageBottom(r, facfg);
}

//show search result
void topic_search_result(request_rec* r, af_app_config* facfg,
					     const char* board_bname, signal_board_info sbi,
					     apr_table_t* param_list)
{
	tpl_t* my_tpl;
	char* content = 0;
	
	char tmp_str[MEDIM_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int ret = 0;
	int rec_num = 0;
	int offset = 0;
	int rec_per_page = 0;
	int int_page = 0;

	const char* key = 0;
	const char* page = 0;
	const char* suit_records = 0;

	const char* title = 0;
	const char* eximg = 0;
	char express_img[SMALL_MEM_SIZE] = {0};
	char opt_url[MEDIM_MEM_SIZE] = {0};

	//get current param
	key = ap_table_get(param_list, "k");
	page = ap_table_get(param_list, "p");

	//no keyword for search
	if(!key || strlen(key) <= 0)
	{
		memset( tmp_str, 0, sizeof( tmp_str));
		snprintf( tmp_str, sizeof( tmp_str) -1, "%s/%s", facfg->appRunUrl, board_bname);
		 output_msgPage(r, facfg, ucocfg, "Need key word.", tmp_str, 0);
		return;
	}

	//show page header
	output_pageHeader(r, facfg, sbi.name,  ucocfg);

	int_page = (page && strcmp(page, "(null)")) ? atoi(page) : 1;
	rec_per_page = facfg->appRecPePage ? atoi(facfg->appRecPePage) : 15;
	offset = (int_page - 1) * rec_per_page;

	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, FORUM_TPL_DIR, FORUM_SEARCH_RESULT_TPL);

	//set general variable
	gen_pub_varible_set(r, facfg, &my_tpl);
	tpl_set_field_fmt_global(my_tpl, "show_board_bname", "%s", board_bname ? board_bname : sbi.id);
	tpl_set_field_fmt_global(my_tpl, "show_board_name", "%s", sbi.name ? sbi.name : "");
	tpl_set_field_fmt_global(my_tpl, "show_key", "%s", key ? key : "");

	//get rand mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	//calcate suit topic number
	memset(tmp_str, 0, sizeof(tmp_str));
	snprintf(tmp_str, sizeof(tmp_str) - 1, "select count(*) from af_content_%s where title like '%%%s%%'", sbi.id,  key);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], tmp_str, &errBuf);

	if(ret)
	{
		rec_num = mysqlNumRows(&global_af_handler[rand_mh]);
		global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]);
		suit_records = global_af_handler[rand_mh].pRow[0];
	}

	if(suit_records && atoi(suit_records)  >  0)
	{
		//get suit topic data
		mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], "SET NAMES 'GB2312'", &errBuf);
		memset(tmp_str, 0, sizeof(tmp_str));
		snprintf(tmp_str, sizeof(tmp_str) - 1, "select id, title, express,  rid, poster, posterId, \
				 postTime from af_content_%s  where title like '%%%s%%' limit %d, %d", sbi.id, key, offset, rec_per_page);
	
		ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], tmp_str, &errBuf);

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
				
					tpl_set_field_fmt(my_tpl, "show_rootTopic_id", "%s", global_af_handler[rand_mh].pRow[3] ? global_af_handler[rand_mh].pRow[3] : "0");
				
					tpl_set_field_fmt(my_tpl, "show_poster", "%s", global_af_handler[rand_mh].pRow[4] ? global_af_handler[rand_mh].pRow[4] : "");
					tpl_set_field_fmt(my_tpl, "show_poster_id", "%s", global_af_handler[rand_mh].pRow[5] ? global_af_handler[rand_mh].pRow[5] : "");
					tpl_set_field_fmt(my_tpl, "show_post_time", "%s", global_af_handler[rand_mh].pRow[6] ? global_af_handler[rand_mh].pRow[6] : "");

					tpl_append_section(my_tpl);
				}
			
				//free result
				mysqlFreeResult(&global_af_handler[rand_mh]);

				tpl_deselect_section(my_tpl);
			}
		}
	}

	//general page list
	snprintf(opt_url, sizeof(opt_url) - 1, "%s?cmd=query&s=2&k=%s&p=", board_bname, key);
	general_page_list(r, &my_tpl, opt_url, 1, atoi(suit_records), rec_per_page, int_page);

	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);

	//ouput page bottom
	output_pageBottom(r, facfg);
}

