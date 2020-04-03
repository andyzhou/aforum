/*
 * Copyright(c)2005
 * All rights reserved.
 *
 * file name:fun_forum_reg_user.c
 * description:registe new user from union
 *
 * author:zhirui
 * create date:2007-5-20
 * version:0.1
 */


#include <stdio.h>
#include <string.h>

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_main.h"
#include "http_log.h"
#include "http_protocol.h"
#include "ap_alloc.h"

#ifdef WIN32
#include "../../_lib/apreq/apache_request.h"
#include "../../_lib/custom/api_flate.h"
#include "../../_lib/custom/api_util.h"
#include "../../_lib/custom/api_mysql.h"
#include "../../_pubfunc/fun_public.h"
#include "../public/fun_forum_cfg.h"
#include "../public/fun_forum_public.h"
#else
#include "apache_request.h"
#include "api_flate.h"
#include "api_util.h"
#include "api_mysql.h"
#include "fun_public.h"
#include "fun_forum_cfg.h"
#include "fun_forum_public.h"
#endif

#include "fun_forum_reg_user.h"


//注册新会员到论坛用户表
void reg_user_from_union(request_rec *r, forum_app_config* facfg,
						 table* param_list)
{
	char sql_str[256] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	FILE *fp = NULL;
	const char* userFileKey = 0;
	char data_file_str[64] = {0};
	char data_str[256] = {0};

	array_header* user_info_arr;//用户信息数组
	char** tmp_data;
	char ** uinfo_list;
	int i = 0;

	userFileKey = ap_table_get(param_list, "2");

	if(!userFileKey || !(*userFileKey))
	{
		//无效页面显示
		forum_invalid_page(r, facfg);
		return;
	}

	//打开文件并读取数据
	memset(data_file_str, 0, sizeof(data_file_str));
	snprintf(data_file_str, sizeof(data_file_str) - 1, "/tmp/%s", userFileKey);

	fp = fopen(data_file_str, "r");

	if(!fp)
	{
		//无效页面显示
		forum_invalid_page(r, facfg);
		return;
	}


	//r->content_type = "text/html";
	//ap_send_http_header(r);

	fgets(data_str, sizeof(data_str) - 1, fp);

	fclose(fp);

	//分析数据
	i = splitStr(r, data_str, '|', &user_info_arr);

	if(i < 4)
	{
		//无效页面显示
		forum_invalid_page(r, facfg);
		return;
	}

	uinfo_list = (char**)user_info_arr->elts;

	/*
	ap_rprintf(r, "<h1>uid:%s, unick:%s, uemail:%s, usex:%s</h1>",
				  uinfo_list[0], uinfo_list[1], uinfo_list[2], uinfo_list[3]);
	*/


	//随机获取mysql句柄
	rand_mh = get_rand_mysql_number(FORUM_MYSQL_SERVICE_NUM);

	//注册新用户数据到论坛用户表
	memset(sql_str, 0, sizeof(sql_str));
	sprintf(sql_str, "insert into af_member(id, name, email, sex, regDate) \
					  values(%d, '%s', '%s', %d, now())", atoi(uinfo_list[0]),
					  uinfo_list[1], uinfo_list[2], atoi(uinfo_list[3]));

	ret = mysqlSimpQuery(r->pool, &global_forum_handler[rand_mh], sql_str, &errBuf);

	if(!ret)
	{
		//错误页面重定向
		show_forum_err_msg(r, facfg, errBuf, facfg->appUrl, 1);
		return;
	}

	//删除临时文件
	unlink(data_file_str);

	//内部跳转
	ap_internal_redirect(facfg->appRunUrl, r);
}

