/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_public.h
* file desc£ºpublic function define
* author£ºAndyZhou<diudiu8848@163.com>
*/

#ifndef _FUN_PUBLIC_H
#define _FUN_PUBLIC_H


#ifdef WIN32
#include "mysql/mysql.h"
#include "../lib20/custom/api_mysql.h"
#else
#include "mysql.h"
#include "api_mysql.h"
#endif

//define gerneral memory size
#define TINY_MEM_SIZE 32
#define SMALL_MEM_SIZE 128
#define MEDIM_MEM_SIZE 512
#define BIG_MEM_SIZE 1024

//define mysql connect handler number
#define MYSQL_SERVICE_NUM 3 //must be odd number

//moderater number per board
#define MODERATERS_PER_BOARD 5

//mysql handler array
mysql_handle global_af_handler[MYSQL_SERVICE_NUM];


//define tpl sub path 
#define PUBLIC_TPL_DIR "public"
#define UNION_TPL_DIR "member"
#define FORUM_TPL_DIR "forum"
#define SYS_TPL_DIR "sys"

#define SYS_INTERNAL_RUN_NAME_1 "sys"
#define SYS_INTERNAL_RUN_NAME_2 "union"
#define SYS_INTERNAL_RUN_NAME_3 "search"

#define DEF_COOKIE_NAME "aforum"


//define tmp path for uploaded files
#ifdef WIN32
#define UPLOAD_FILE_TMP_DIR "C:\\windows"
#else
#define UPLOAD_FILE_TMP_DIR "/tmp"
#endif

//define top topic last post time begin
#define TOP_TOPIC_TIME_BEGIN 5
#define TOP_TOPIC_TIME_VALUE 1200000000 //toped
#define UNTOP_TOPIC_TIME_VALUE 1190000000 //untoped


//user cookie info struct
typedef struct {
	int uid;//cookie user id
	char* nick;//cookie user nick
	char* email;//cookie useremail
	int type;//cookie user type
	int available;//is available
} unionCookieInfo;


//user basic info struct
typedef struct {
	char	uid[10];//uid
	char	nick[32];//user nick
	char	type[10];//user type
} unionBasicInfo;


//db config
db_cfg af_db_cfg;

//user cookie info
unionCookieInfo ucocfg;

//////////////////////////////////////////

extern af_app_config *acfg;

//init db config
void af_db_cfg_init(af_app_config* acfg, db_cfg* cfg);


//get rand number
int get_rand_mysql_number(int end_num);


//conver utf8->gb2312 or gb2312->utf8
int code_convert(char *from_charset, char *to_charset,
				 	  char *inbuf, int inlen, 
				 	  char *outbuf,int outlen);

//output standard text/html
void output_header(request_rec *r);

//ajax data output
void output_ajax_txt_header(request_rec* r, char* txt_str);


//tpl file init
void tpl_file_init(request_rec* r, af_app_config *facfg, tpl_t **tpl, 
			     		 const char* tpl_dir, const char* tpl_file);

//public tpl varibale operate
void gen_pub_varible_set(request_rec* r, af_app_config *facfg,
						 		 tpl_t **my_tpl);

//union generic page header
void output_pageHeader(request_rec* r, af_app_config *facfg,
					   	  char* title, unionCookieInfo ucocfg);

//union page bottom
void output_pageBottom(request_rec* r, af_app_config *facfg);


//show simple info for pop page
void output_simplePage(request_rec* r, af_app_config *facfg, char* msg);


//show system message page
void output_msgPage(request_rec* r, af_app_config *facfg,
						  unionCookieInfo ucocfg, char* msg,
					     char* url, int ok_failed);




//show invalid page
void output_invalid_page(request_rec* r, af_app_config *facfg);

//general page list 
void general_page_list(request_rec* r, tpl_t **my_tpl,
					   	  const char* opt_url, int page_style,
					   	  int total_record, int record_per_page,
					   	  int cur_page);


//get current user cookie info
int get_general_cookie(request_rec* r, af_app_config *facfg, unionCookieInfo* member_ci);


//get file extend name
int get_file_ext_name(char* extName, const char* lpCType);


//save signal uploaded file 1:success 0:failed
int save_signal_upload_file(request_rec* r, char* source_file, int source_file_size, 
									 char* save_target_file);

/////////////////////

#endif

