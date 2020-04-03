/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name：fun_public.c
* file desc：public functions
* author：AndyZhou<diudiu8848@163.com>
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
#include <iostream.h>

#ifdef WIN32
#else
#include "iconv.h"
#endif
*/

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


#ifdef WIN32
#include "../lib20/apreq/apache_cookie.h"
#include "../lib20/custom/api_util.h"
#else
#include "apache_cookie.h"
#include "api_util.h"
#endif

#include "fun_cfg.h"
#include "tpllib.h"
#include "fun_public.h"

//////////////////////


//init db config
void af_db_cfg_init(af_app_config* acfg, db_cfg* cfg)
{
	//init db config
	sprintf(cfg->db_server, "%s", acfg->appDbServ);
	sprintf(cfg->db_user, "%s", acfg->appDbUser);
	sprintf(cfg->db_pwd, "%s", acfg->appDbPwd);
	sprintf(cfg->db_name, "%s", acfg->appDbName);
	cfg->db_port = atoi(acfg->appDbPort);
}


//get rand number
int get_rand_mysql_number(int end_num)
{
	
	int rand_num = 0;
	int rand_time = 0;
	time_t timeNow;
	
	//get rand time
	srand(time(&timeNow));
	
	//generate rand number
	rand_time = rand();
	rand_num = rand_time % end_num;
	//rand_num = rand_time & end_num;

	return rand_num;
}


/*
//utf8->gb2312 gb2312->utf8
int code_convert(char *from_charset, char *to_charset,
				 char *inbuf, int inlen, 
				 char *outbuf,int outlen)
{
	iconv_t cd;
	int rc;
	char **pin = &inbuf;
	char **pout = &outbuf;
	
	cd = iconv_open(to_charset,from_charset);
	if (cd==0) return -1;
	memset(outbuf,0,outlen);
	if (iconv(cd,pin,&inlen,pout,&outlen)==-1) return -1;
	iconv_close(cd);
	return 0;
}
*/

void output_header(request_rec *r)
{
	r->content_type = "text/html";
	ap_send_http_header(r);
}


//output ajax text data
void output_ajax_txt_header(request_rec* r, char* txt_str)
{	
	r->content_type = "text/html";
	ap_send_http_header(r);
	ap_rprintf(r, "%s", txt_str);
}


//tpl file init
void tpl_file_init(request_rec* r, af_app_config *facfg, tpl_t **tpl, 
			     		 const char* tpl_dir, const char* tpl_file)
{	
	char* tpl_file_name = 0;

	//init tpl engine
	*tpl = tpl_alloc();

	tpl_file_name = (char*)ap_palloc(r->pool, MEDIM_MEM_SIZE);
	tpl_file_name = (char*)ap_pcalloc(r->pool, MEDIM_MEM_SIZE);

	tpl_file_name = ap_psprintf(r->pool, "%s/%s/%s", facfg->appTplPath, tpl_dir, tpl_file);

	//ap_rprintf(r, "<h1>tpl_file_name:%s</h1>", tpl_file_name);

	//load tpl file
	tpl_load(r, *tpl, tpl_file_name);
}


//public tpl varibale operate
void gen_pub_varible_set(request_rec* r, af_app_config *facfg,
						   	 tpl_t **my_tpl)
{
	tpl_set_field_fmt_global(*my_tpl, "app_url", "%s", facfg->appUrl);
	tpl_set_field_fmt_global(*my_tpl, "site_url", "%s", facfg->appUrl);
	tpl_set_field_fmt_global(*my_tpl, "app_name", "%s", facfg->appName);
	tpl_set_field_fmt_global(*my_tpl, "app_run_url", "%s", facfg->appRunUrl);
}


//generic page header
void output_pageHeader(request_rec* r, af_app_config *facfg,
					   	  char* title, unionCookieInfo ucocfg)
{
	tpl_t *my_tpl;
	char tpl_file[SMALL_MEM_SIZE] = {0};
	char *content = 0;

	ap_set_content_type(r, "text/html");

	if(ucocfg.available)
	{
		//user logined page header
		sprintf(tpl_file, "%s", "af_page_lheader.html");
	}
	else
	{
		sprintf(tpl_file, "%s", "af_page_uheader.html");
	}

	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, PUBLIC_TPL_DIR, tpl_file);

	tpl_set_field_fmt_global(my_tpl, "item_name", "%s", title ? title : "");
	tpl_set_field_fmt_global(my_tpl, "user_nickname", "%s", ucocfg.nick ? ucocfg.nick : "");

	//set global tpl variable
	gen_pub_varible_set(r, facfg, &my_tpl);

	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);
}


//generic page bottom
void output_pageBottom(request_rec* r, af_app_config *facfg)
{
	tpl_t *my_tpl;
	char *content = 0;

	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, PUBLIC_TPL_DIR, "af_page_bottom.html");

	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);
}


//show simple info for pop page
void output_simplePage(request_rec* r, af_app_config *facfg, char* msg)
{
	tpl_t *my_tpl;
	char *content = 0;

	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, PUBLIC_TPL_DIR, "af_simple_msg.html");

	//set global tpl variable
	gen_pub_varible_set(r, facfg, &my_tpl);

	tpl_set_field_fmt_global(my_tpl, "show_sys_msg", "%s", msg ? msg : "");

	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);
}


//show system message page
//ok_failed 0:failed 1:ok
void output_msgPage(request_rec* r, af_app_config *facfg,
					unionCookieInfo ucocfg, char* msg,
					char* url, int ok_failed)
{
	tpl_t *my_tpl;
	char *content = 0;
	char title[32] = {0};
	char tplfile[64] = {0};

	if(ok_failed == 1)
	{
		sprintf(title, "%s", "Success");
		sprintf(tplfile, "%s", "af_success.html");
	}
	else
	{
		sprintf(title, "%s", "Failed");
		sprintf(tplfile, "%s", "af_error.html");
	}

	//page header
	output_pageHeader(r, facfg, title, ucocfg);

	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, PUBLIC_TPL_DIR, tplfile);

	//set global tpl variable
	gen_pub_varible_set(r, facfg, &my_tpl);

	tpl_set_field_fmt_global(my_tpl, "show_sys_msg", "%s", msg ? msg : "");
	tpl_set_field_fmt_global(my_tpl, "show_return_url", "%s", url ? url : facfg->appRunUrl);

	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);


	//page bottom
	output_pageBottom(r, facfg);
}



//show invalid page
void output_invalid_page(request_rec* r, af_app_config *facfg)
{
	tpl_t *my_tpl;
	char *content = 0;
	
	//page header
	output_pageHeader(r, facfg, "无效页面", ucocfg);

	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, PUBLIC_TPL_DIR, "af_invalid_page.html");

	//set global tpl variable
	gen_pub_varible_set(r, facfg, &my_tpl);

	//get tpl content
	content = apr_palloc(r->pool, tpl_length(my_tpl) + 1);
	tpl_get_content(my_tpl, content);

	//output tpl content
	ap_rprintf(r, "%s", content);
	tpl_free(my_tpl);
	
	//page bottom
	output_pageBottom(r, facfg);
}


//general page list 
void general_page_list(request_rec* r, tpl_t **my_tpl,
					   	  const char* opt_url, int page_style,
					   	  int total_record, int record_per_page,
					   	  int cur_page)
{
	char tmp_str[64] = {0};
	char new_page_list[512] = {0};
	int int_total_pages = 0;

	int page_begin = 0;
	int page_end = 0;
	int page_next = 0;
	int page_prev = 0;

	int page_nums = 3;//page no numbers per time
	int i = 0;


	//ap_rprintf(r, "<h1>record_per_page:%d, opt_url:%s</h1>", record_per_page, opt_url);

	if(!opt_url || !(*opt_url))
	{
		return;
	}

	if(total_record <= 0)
	{
		total_record = 1;
	}

	if(record_per_page <= 0)
	{
		return;
	}

	if(cur_page <= 1)
	{
		cur_page = 1;
	}

	if(total_record <= record_per_page)
	{
		int_total_pages = 1;
	}
	else
	{
		int_total_pages = (int)total_record/record_per_page;

		if(int_total_pages * record_per_page != total_record)
		{
			int_total_pages++;
		}
	}

	page_begin	= cur_page - page_nums;
	page_end	= cur_page + page_nums;

	if(page_begin < 1)
	{
		page_begin = 1;
		page_end = 2 * page_nums;
	}
	
	if(page_end > int_total_pages)
	{
		page_end = int_total_pages;
	}

	if(page_end == int_total_pages && int_total_pages > 1)
	{
		if(page_begin > (int_total_pages - 2 * page_nums))
		{
			page_begin = int_total_pages - 2 * page_nums;
		}

		if(page_begin < 1)
		{
			page_begin = 1;
		}
	}

	page_prev = cur_page - 1;
	page_next = cur_page + 1;


	//ap_rprintf(r, "<h1>int_total_pages:%d</h1>", int_total_pages);

	//total records
	memset(tmp_str, 0, sizeof(tmp_str));
	sprintf(tmp_str, "%d", total_record);
	tpl_set_field_fmt_global(*my_tpl, "total_records", "%s", tmp_str);

	//total pages
	memset(tmp_str, 0, sizeof(tmp_str));
	sprintf(tmp_str, "%d", int_total_pages);	
	tpl_set_field_fmt_global(*my_tpl, "total_pages", "%s", tmp_str);

	//current page no
	memset(tmp_str, 0, sizeof(tmp_str));
	sprintf(tmp_str, "%d", cur_page);	
	tpl_set_field_fmt_global(*my_tpl, "cur_page", "%s", tmp_str);

	if(cur_page > 1)
	{
		//prev page
		memset(new_page_list, 0, sizeof(new_page_list));
		sprintf(new_page_list, "<a href=%s%d>Prev</a>", opt_url, page_prev);
		tpl_set_field_fmt_global(*my_tpl, "prev_page", "%s", new_page_list);
	}
	
	if(cur_page >= 1 && (int_total_pages - cur_page) >= 1)
	{
		//next page
		memset(new_page_list, 0, sizeof(new_page_list));
		sprintf(new_page_list, "<a href=%s%d>Next</a>", opt_url, page_next);
		tpl_set_field_fmt_global(*my_tpl, "next_page", "%s", new_page_list);
	}


	memset(new_page_list, 0, sizeof(new_page_list));
	if(page_begin > page_nums)
	{
		sprintf(new_page_list, "%s", "... ");
	}

	//page no list
	for(i = page_begin; i <= page_end; i++)
	{
		memset(tmp_str, 0, sizeof(tmp_str));

		if(i == cur_page)
		{
			snprintf(tmp_str, sizeof(tmp_str) - 1, "%d ", i);
		}
		else
		{
			snprintf(tmp_str, sizeof(tmp_str) - 1, "<a href=%s%d>%d</a> ", opt_url, i, i);
		}

		if(page_begin > page_nums)
		{
			strcat(new_page_list, tmp_str);
		}
		else
		{
			if(i == page_begin)
			{
				sprintf(new_page_list, tmp_str);
			}
			else
			{
				strcat(new_page_list, tmp_str);
			}
		}
	}

	if(page_begin > page_nums)
	{
		strcat(new_page_list, " ...");
	}

	tpl_set_field_fmt_global(*my_tpl, "page_list", "%s", new_page_list);
}



/*
//初始化cookie动态变量
void union_cookie_cfg_init(const char* appCookieName)
{
	sprintf(union_uid_cookie_name,	  "%s_uid",		appCookieName);
	sprintf(union_unick_cookie_name,  "%s_unick",	appCookieName);
	sprintf(union_uename_cookie_name, "%s_uename",	appCookieName);
	sprintf(union_uhash_cookie_name,  "%s_uhash",	appCookieName);
}


*/



//get current user cookie info
int get_general_cookie(request_rec* r, af_app_config *facfg, unionCookieInfo* member_ci)
{
	int i = 0;
	int j = 0;
	int num = 0;

	char *name = 0;
	char *value = 0;
	char ** array_enum_list;

	apr_array_header_t *arrList;
	char tmp_cookie_list[MEDIM_MEM_SIZE] = {0};

	ApacheCookieJar *cookies = ApacheCookie_parse(r, NULL);

	memset(member_ci, 0, sizeof(unionCookieInfo));

	member_ci->available = 0;
	member_ci->uid = 0;

	member_ci->nick = (char*)ap_palloc(r->pool, SMALL_MEM_SIZE);
	member_ci->nick = (char*)ap_pcalloc(r->pool, SMALL_MEM_SIZE);

	member_ci->email = (char*)ap_palloc(r->pool, SMALL_MEM_SIZE);
	member_ci->email = (char*)ap_pcalloc(r->pool, SMALL_MEM_SIZE);


	for (i = 0; i < ApacheCookieJarItems(cookies); i++)
	{ 
		ApacheCookie *c = ApacheCookieJarFetch(cookies, i);
     
		for (j = 0; j < ApacheCookieItems(c); j++)
		{
			name = c->name;
			value = ApacheCookieFetch(c, j);

			//get suit cookie name and value
			if(name && !strcmp(facfg->appCookieName ? facfg->appCookieName : DEF_COOKIE_NAME, name) && (value && strlen(value) > 1))
			{
				//find suit cookie and decode it
				apr_base64_decode(tmp_cookie_list, value);

				if(strlen(tmp_cookie_list) > 0)
				{
					//split cookie list to array
					num = splitStr(r, tmp_cookie_list, '|', &arrList);

					if(num < 3)
					{
						return 0;
					}

					//store cookie data to struct
					array_enum_list = (char**)arrList->elts;
					
					member_ci->uid = atoi(array_enum_list[0]);
					snprintf(member_ci->nick, SMALL_MEM_SIZE - 1, "%s", array_enum_list[1]);
					snprintf(member_ci->email, SMALL_MEM_SIZE - 1, "%s", array_enum_list[2]);
					member_ci->type = atoi(array_enum_list[3] ? array_enum_list[3] : "0");
					member_ci->available = 1;

					return 1;
				}

				return 0;
			}
		}
     }

	return 0;
}


//save signal uploaded file 1:success 0:failed
int save_signal_upload_file(request_rec* r, char* source_file, int source_file_size, 
									 char* save_target_file)
{
	apr_status_t rv;
	apr_file_t* src_fp;
	apr_file_t* fp;

	int tmp_size = 0;
	char* filecontent = 0;

	if(!source_file)
	{
		return 0;
	}

	if(source_file_size <= 0)
	{
		return 0;
	}

	if(!save_target_file)
	{
		return 0;
	}

	//read source file
	r->filename = ap_psprintf(r->pool, source_file);
	rv = apr_file_open(&src_fp, r->filename, APR_READ, APR_UREAD | APR_GREAD, r->pool);

	if(rv != APR_SUCCESS)
	{
		return 0;
	}

	filecontent = (char*)apr_palloc(r->pool, source_file_size + 1);
	filecontent = (char*)apr_pcalloc(r->pool, source_file_size + 1);

	apr_file_read(src_fp, filecontent, &source_file_size);
	
	if(strlen(filecontent) <= 0)
	{
		 apr_file_close(src_fp);
		 return 0;
	}

	//open dst file for write
	rv = apr_file_open(&fp, save_target_file, APR_CREATE|APR_WRITE|APR_BINARY|APR_TRUNCATE, APR_OS_DEFAULT, r->pool);

	if(rv != APR_SUCCESS)
	{
		apr_file_close(src_fp);
		return 0;
	}

	apr_file_write(fp, filecontent, &source_file_size);
	
	apr_file_close(src_fp);
	apr_file_close(fp);

	return 1;
}





