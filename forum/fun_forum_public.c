/*
* Copyright (c) 2007
* All rights reserved.
* 
* file name£ºfun_forum_public.c
* file desc£ºforum public funcs
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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const char *g_CTExt[] = {
//-----------------------------------------------------------------------
//  Content-Type			Extension:Description
//-----------------------------------------------------------------------
	"image/bmp",						"bmp\0",//Windows bitmap",
	"image/cgm",						"cgm\0",//Computer Graphics Metafile",
	"image/fif",						"fif\0",//Fractal Image Format image",
	"image/g3fax",						"g3f\0",//Group III FAX image",
	"image/gif",						"gif\0",//GIF image",
	"image/jpeg",						"jpg\0",//JPEG image",
	"image/pjpeg",						"jpg\0",//JPEG image",
	"image/png",						"png\0",//PNG image",
	"image/x-png",						"png\0",//PNG image",
	"image/tiff",						"tiff\0",//TIFF image",
	"image/x-xbitmap",				"xbm\0",//X bitmap",
	"image/x-xbm",						"xbm\0",//X bitmap",
	"image/x-xpixmap",				"xpm\0",//X pixmap",
	"image/x-xpm",						"xpm\0",//X pixmap",
};

////////////////////////////////

//get signal board simple info
//1:ok 0:failed
int get_signal_board_info(request_rec* r, char* board_bname, signal_board_info* sbi)
{
	char sql_str[MEDIM_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	//get random mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	//query data
	memset(sql_str, 0, sizeof(sql_str));
	snprintf(sql_str, sizeof(sql_str) - 1, "select id, title, topics, posts, intro, isOpen, browLevel, postLevel \
					  from af_boards where bname = '%s' limit 1", board_bname);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);
	
	if(!ret)
	{
		return 0;
	}

	rec_num = mysqlNumRows(&global_af_handler[rand_mh]);

	if(rec_num <= 0)
	{
		return 0;
	}

	global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]);

	snprintf(sbi->id, sizeof(sbi->id) - 1, "%s", global_af_handler[rand_mh].pRow[0]);
	snprintf(sbi->name, sizeof(sbi->name) - 1, "%s", global_af_handler[rand_mh].pRow[1]);
	snprintf(sbi->topics, sizeof(sbi->topics) - 1, "%s", global_af_handler[rand_mh].pRow[2]);
	snprintf(sbi->posts, sizeof(sbi->posts) - 1, "%s", global_af_handler[rand_mh].pRow[3]);
	snprintf(sbi->intro, sizeof(sbi->intro) - 1, "%s", global_af_handler[rand_mh].pRow[4]);
	
	sbi->isOpen		 = atoi(global_af_handler[rand_mh].pRow[5]);
	sbi->browseLevel = atoi(global_af_handler[rand_mh].pRow[6]);
	sbi->postLevel	 = atoi(global_af_handler[rand_mh].pRow[7]);

	//free result
	mysqlFreeResult(&global_af_handler[rand_mh]);

	return 1;
}


//get signal root topic info
//1:ok 0:failed
int get_signal_root_topic_info(request_rec* r, int board_id,
							   int topic_id, signal_root_topic_info* srti)
{
	char sql_str[MEDIM_MEM_SIZE] = {0};
	char* errBuf = 0;
	int rand_mh = 0;
	int rec_num = 0;
	int ret = 0;

	if(board_id <= 0 || topic_id <= 0)
	{
		return 0;
	}

	//get random mysql handler
	rand_mh = get_rand_mysql_number(MYSQL_SERVICE_NUM);

	//query data
	memset(sql_str, 0, sizeof(sql_str));
	snprintf(sql_str, sizeof(sql_str) - 1, "select title, reply, locked, hot, lastPost \
			 from af_topic_%d where id = %d limit 1", board_id, topic_id);

	ret = mysqlSimpQuery(r->pool, &global_af_handler[rand_mh], sql_str, &errBuf);
	
	if(!ret)
	{
		return 0;
	}

	rec_num = mysqlNumRows(&global_af_handler[rand_mh]);

	if(rec_num <= 0)
	{
		return 0;
	}

	global_af_handler[rand_mh].pRow = mysqlFetchRow(&global_af_handler[rand_mh]);

	snprintf(srti->title, sizeof(srti->title) - 1, "%s", global_af_handler[rand_mh].pRow[0]);
	srti->reply		= atoi(global_af_handler[rand_mh].pRow[1]);
	srti->locked	= atoi(global_af_handler[rand_mh].pRow[2]);
	srti->hoted	= atoi(global_af_handler[rand_mh].pRow[3]);

	if(atoi(global_af_handler[rand_mh].pRow[4]) >= TOP_TOPIC_TIME_VALUE)
	{
		srti->toped = 1;
	}
	else
	{
		srti->toped = 0;
	}

	//free result
	mysqlFreeResult(&global_af_handler[rand_mh]);

	return 1;
}


//get file extend name
int get_file_ext_name(char* extName, const char* lpCType)
{
	int i = 0;

	if(!lpCType)
	{
		return 0;
	}

	for(i=0; i< sizeof(g_CTExt)/sizeof(g_CTExt[0]); i+=2)
	{
		if(!strcmp(lpCType, g_CTExt[i]))
		{
			extName[0]='.';
			strcpy(extName+1, g_CTExt[i+1]) - 1;
			return 1;
		}
	}

	return 0;
}



