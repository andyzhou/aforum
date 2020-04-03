/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name：fun_union_profile.c
* file desc：member edit profile
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
#include "apache_cookie.h"

#include "fun_cfg.h"
#include "tpllib.h"
#include "fun_public.h"

#include "fun_union_profile.h"


#define UNION_PROFILE_EDIT_TPL "af_edit_prof.html"


//edit profile
void member_profile_edit(request_rec* r, af_app_config *facfg)
{
	tpl_t *my_tpl;
	char* content = 0;

	char tmp_str[BIG_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	const char* photo = 0;
	int has_photo = 0;

	//show page header
	output_pageHeader(r, facfg, "Personal Info",  ucocfg);


	//tpl file init
	tpl_file_init(r, facfg, &my_tpl, UNION_TPL_DIR, UNION_PROFILE_EDIT_TPL);

	//set general variable
	gen_pub_varible_set(r, facfg, &my_tpl);

	tpl_set_field_fmt_global(my_tpl, "member_email", "%s", ucocfg.email);

	//get current user profile for db
	snprintf(tmp_str, sizeof(tmp_str) - 1, "select sex, location, signInfo, myPhoto from af_member where id = %d limit 1", ucocfg.uid);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], tmp_str, &errBuf);

	if(ret)
	{
		rec_num = mysqlNumRows(&global_af_handler[rand_mh]);

		if(rec_num > 0)
		{	
			global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]);

			switch(atoi(global_af_handler[rand_mh].pRow[0]))
			{
				case 1:
					tpl_set_field_fmt_global(my_tpl, "sex_1", "%s", "checked");
				break;

				case 2:
					tpl_set_field_fmt_global(my_tpl, "sex_2", "%s", "checked");
				break;

				default:
					tpl_set_field_fmt_global(my_tpl, "sex_0", "%s", "checked");
				break;
			}

			photo = global_af_handler[rand_mh].pRow[3];

			if(photo && strlen(photo) > 0)
			{
				memset(tmp_str, 0, sizeof(tmp_str));
				snprintf(tmp_str, sizeof(tmp_str) - 1, "<img src=\"%s/%s\">", facfg->appUploadUrl, photo);

				tpl_set_field_fmt_global(my_tpl, "member_photo", "%s", tmp_str);
			}

			tpl_set_field_fmt_global(my_tpl, "location", "%s", global_af_handler[rand_mh].pRow[1]);
			tpl_set_field_fmt_global(my_tpl, "signinfo", "%s", global_af_handler[rand_mh].pRow[2]);

			//free result 
			mysqlFreeResult(&global_af_handler[rand_mh]);
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


//save profile
void member_profile_save(request_rec* r, af_app_config *facfg, ApacheRequest *req)
{
	char locationUrl[MEDIM_MEM_SIZE] = {0};
	char sql_str[BIG_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	ApacheUpload *upload_files;
	const char* tmp_file_type = 0;
	char tmp_file_ext[TINY_MEM_SIZE] = {0};
	int file_no = 0;

	char tmp_str[MEDIM_MEM_SIZE] = {0};
	FILE* tag_img_fp = NULL;
	int upload_result = 0;

	const char* myPhoto = 0;
	const char* sex = 0;
	const char* location = 0;
	const char* signInfo = 0;

	//get input param 
	myPhoto		= ApacheRequest_param(req, "myPhoto");
	sex			= ApacheRequest_param(req, "sex");
	location		= ApacheRequest_param(req, "location");
	signInfo		= ApacheRequest_param(req, "signInfo");

	memset(locationUrl, 0, sizeof(locationUrl));
	sprintf(locationUrl, "%s/union/my/edit", facfg->appRunUrl);


	//ap_set_content_type(r, "text/html");
	//ap_rprintf(r, "<h1>UPLOAD_FILE_TMP_DIR:%s</h1>", UPLOAD_FILE_TMP_DIR);


	//upload photo ?
	for(upload_files = ApacheRequest_upload(req), file_no = 1; upload_files;
		upload_files = upload_files->next, file_no++)
	{
		//ap_rprintf(r, "<h1>file:%s</h1>", upload_files->tempname);
		//ap_rprintf(r, "<h1>size:%s</h1>", upload_files->tmpSize);

		if(atol(upload_files->tmpSize) <= 0)
		{
			continue;
		}

		if(atol(upload_files->tmpSize) > (1024 * 10))
		{
			output_msgPage(r, facfg, ucocfg, "个人照片超出10KB，请重试", locationUrl, 0);
			return;			
		}

		//get file extend name
		memset(tmp_file_ext, 0, sizeof(tmp_file_ext));
		tmp_file_type = (char*)ap_table_get(upload_files->info, "Content-type");
		get_file_ext_name(tmp_file_ext, tmp_file_type);

		//ap_rprintf(r, "<h1>size:%s, ext:%s</h1>", upload_files->tmpSize, tmp_file_ext);

		if(strlen(tmp_file_ext) > 2)
		{
			//only save photo
			if(!strcmp(tmp_file_ext, ".gif") || !strcmp(tmp_file_ext, ".jpg") || !strcmp(tmp_file_ext, ".png"))
			{
				memset(tmp_str, 0, sizeof(tmp_str));
				snprintf(tmp_str, sizeof(tmp_str) - 1, "%s/%d%s",  facfg->appUploadPath, ucocfg.uid, tmp_file_ext);

				//save user photo file to dest dir
				upload_result = save_signal_upload_file(r, upload_files->tempname, atol(upload_files->tmpSize), tmp_str);

				
				memset(tmp_str, 0, sizeof(tmp_str));
				if(upload_result)
				{
					//format user photo sql
					snprintf(tmp_str, sizeof(tmp_str) - 1, ", myPhoto = '%d%s'", ucocfg.uid, tmp_file_ext);
				}

				//unlink src image file
				unlink(upload_files->tempname);

				break;
			}
		}
	}

	//update user profile
	snprintf(sql_str, sizeof(sql_str) - 1, "update af_member set sex = %d, location = '%s', signInfo = '%s' %s where id = %d",
				sex ? atoi(sex) : 0, location ? location : "", signInfo ? signInfo : "", tmp_str, ucocfg.uid);

	//ap_rprintf(r, "<h1>sql_str:%s</h1>", sql_str);


	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);

	if(!ret)
	{
		//insert failed
		output_msgPage(r, facfg, ucocfg, "info save failed, try again", locationUrl, 0);
		return;
	}


	//registe success, relocation to login page.
	output_msgPage(r, facfg, ucocfg, "info save success!", locationUrl, 1);
}


