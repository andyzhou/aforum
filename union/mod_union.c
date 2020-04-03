/*
* Copyright (c) 2007
* All rights reserved.
* 
* 文件名称：mod_union.c
* 文件标识：统一会员模块函数
* 作    者：周志锐(丢丢)
*/

#include "apache_request.h"
#include "api_flate.h"
//#include "api_ldap.h"
#include "fun_public.h"

//#include "fun_union_public.h"
//#include "fun_union_interface.h"
//

#include "fun_union_public.h"
#include "fun_union_cfg.h"
#include "api_mysql.h"
#include "mod_union.h"

//http.conf中union模块配置
//extern app_config *ucfg;

//union config variable of union.cfg
//extern union_app_config *uacfg;

//module define
module MODULE_VAR_EXPORT union_module;

//child process init
static void union_child_init(server_rec *s, pool *p)
{
	int i = 0;
	int ret = 0;
	char *errBuf = 0;

	//LOG记录
	ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_NOTICE,
				 s, "%s: child process init.", __FILE__);

	//初始化cookie动态变量
	union_cookie_cfg_init(uacfg->appCookieName);

	//初始化数据库参数
	union_db_cfg_init(uacfg, &union_db_cfg);

	//初始化主数据库链接
	for(i = 0; i <= UNION_MYSQL_SERVICE_NUM; i++)
	{
		
		if(!global_union_handler[i].isConnect || !global_union_handler[i].pConn)
		{
			ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_NOTICE,
							s, "%s: begin connect mysql server %d.",
							__FILE__, i);

			//链接主数据库
			ret = mysqlSimpConnect(p, union_db_cfg, &global_union_handler[i], &errBuf);

			if(!ret)
			{
				ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_NOTICE,
							s, "%s: connect mysql server %d return:%d, error buf:%s.",
							__FILE__, i, ret, errBuf);
			}

			usleep(5000);
		}
	}
}


//union module hander entry
static int union_handler(request_rec* r)
{
	
	//union_mod_init(r);
	union_interface(r, uacfg);

	/*
	r->content_type = "text/html";
	ap_send_http_header(r);
	r->content_type = "text/html";
	ap_send_http_header(r);	
	ap_rprintf(r, "<h1>appName:%s</h1>", uacfg->appName);

	ap_rprintf(r, "<h1>appName:%s</h1>", uacfg->appName);
	ap_rprintf(r, "<h1>appUrl:%s</h1>", uacfg->appUrl);
	ap_rprintf(r, "<h1>appTplPath:%s</h1>", uacfg->appTplPath);
	ap_rprintf(r, "<h1>appCookieDomain:%s</h1>", uacfg->appCookieDomain);
	ap_rprintf(r, "<h1>appCookieName:%s</h1>", uacfg->appCookieName);

	ap_rprintf(r, "<h1>appDbServ:%s</h1>", uacfg->appDbServ);
	ap_rprintf(r, "<h1>appDbPort:%s</h1>", uacfg->appDbPort);
	ap_rprintf(r, "<h1>appDbSock:%s</h1>", uacfg->appDbSock);
	ap_rprintf(r, "<h1>appDbUser:%s</h1>", uacfg->appDbUser);
	ap_rprintf(r, "<h1>appDbPwd:%s</h1>", uacfg->appDbPwd);
	ap_rprintf(r, "<h1>appDbName:%s</h1>", uacfg->appDbName);


	ap_rprintf(r, "<h1>db serv:%s</h1>", union_db_cfg.db_server);
	ap_rprintf(r, "<h1>db user:%s</h1>", union_db_cfg.db_user);
	ap_rprintf(r, "<h1>db port:%d</h1>", union_db_cfg.db_port);

	
	ap_rprintf(r, "<h1>union_uid_cookie_name:%s</h1>", union_uid_cookie_name);

	if(global_union_handler[0].pConn)
	{
		ap_rputs("<h1>ok</h1>", r);
	}
	else
	{
		ap_rputs("<h1>fail</h1>", r);
	}
	*/

	return OK;
}


//模块调用配置
static handler_rec union_handlers[] = 
{
	{"union", union_handler},
	{NULL}
};


//模块配置表
module MODULE_VAR_EXPORT union_module =
{
		STANDARD_MODULE_STUFF,
		NULL,//union_init,//module init
		NULL,//create_site_config_rec,//create union config	
		NULL,//merge_site_config_rec,//merge_site_config_rec,//merge site config

		create_union_server_config,//server config creator
		merege_union_server_config,//server config merger
		unionServ_cmds,//server cmds
		union_handlers,

		NULL,		
		NULL,
		NULL,
		NULL,
		
		NULL,
		NULL,
		NULL,		
		NULL,

		union_child_init,//NULL, //process initializer
		NULL,		
		NULL
};






