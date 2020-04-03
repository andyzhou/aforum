/*
* Copyright (c) 2007
* All rights reserved.
* 
* 文件名称：fun_union_cfg.c
* 文件标识：服务器端配置操作函数
* 作    者：周志锐(丢丢)
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

#include "api_mysql.h"
#include "fun_union_cfg.h"

//继承union模块变量
extern module MODULE_VAR_EXPORT union_module;


//create union server config
void *create_union_server_config(pool *p, server_rec *s)
{
	union_app_config *rec = (union_app_config *)ap_palloc(p, sizeof(union_app_config));

	//set default value
	rec->appName = "union";
	rec->appUrl  = "localhost";
	rec->appTplPath = "./";
	rec->appCookieDomain = "";
	rec->appCookieName = "union";
	
	rec->appDbServ = "localhost";
	rec->appDbPort = "3306";
	rec->appDbSock = "/tmp/mysql.sock";
	rec->appDbUser = "root";
	rec->appDbPwd  = "";
	rec->appDbName = "";

	return (void *)rec;
}


//merge server config
void *merege_union_server_config(pool *p, void *parent, void *sub)
{	
    union_app_config *par  = (union_app_config *) parent;
    union_app_config *chld = (union_app_config *) sub;
    union_app_config *mrg;
	
	mrg = (union_app_config *) ap_palloc(p, sizeof(union_app_config));

	if(chld->appName)
	{
		mrg->appName = par->appName;
	}
	else
	{
		mrg->appName = chld->appName;
	}
	
	if(chld->appUrl)
	{
		mrg->appUrl = par->appUrl;
	}
	else
	{
		mrg->appUrl = chld->appUrl;
	}
	
	if(chld->appTplPath)
	{
		mrg->appTplPath = par->appTplPath;
	}
	else
	{
		mrg->appTplPath = chld->appTplPath;
	}
	
	if(chld->appCookieDomain)
	{
		mrg->appCookieDomain = par->appCookieDomain;
	}
	else
	{
		mrg->appCookieDomain = chld->appCookieDomain;
	}

	
	if(chld->appCookieName)
	{
		mrg->appCookieName = par->appCookieName;
	}
	else
	{
		mrg->appCookieName = chld->appCookieName;
	}

	/////////////////////


	if(chld->appDbServ)
	{
		mrg->appDbServ = par->appDbServ;
	}
	else
	{
		mrg->appDbServ = chld->appDbServ;
	}
	
	if(chld->appDbPort)
	{
		mrg->appDbPort = par->appDbPort;
	}
	else
	{
		mrg->appDbPort = chld->appDbPort;
	}
	
	if(chld->appDbSock)
	{
		mrg->appDbSock = par->appDbSock;
	}
	else
	{
		mrg->appDbSock = chld->appDbSock;
	}
	
	if(chld->appDbUser)
	{
		mrg->appDbUser = par->appDbUser;
	}
	else
	{
		mrg->appDbUser = chld->appDbUser;
	}
	
	if(chld->appDbPwd)
	{
		mrg->appDbPwd = par->appDbPwd;
	}
	else
	{
		mrg->appDbPwd = chld->appDbPwd;
	}
	
	if(chld->appDbName)
	{
		mrg->appDbName = par->appDbName;
	}
	else
	{
		mrg->appDbName = chld->appDbName;
	}

    return (void *)mrg;
}


//设置服务器端应用名称配置
const char *set_union_serv_app_name(cmd_parms *cmd, void *ct, char *arg)
{
	void *cfgAppName = NULL;

	//Get a pointer to our server config record...
	uacfg = (union_app_config *) ap_get_module_config(cmd->server->module_config, &union_module);

	//Allocate space for config string in the server heap
	if((cfgAppName = ap_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 
			cmd->server,
			"%s: union cfgAppName error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgAppName, arg);

	//Insert pointer to the cfg path string in the server config record
	uacfg->appName = cfgAppName;

	return NULL;
}


//设置服务器端应用URL
const char *set_union_serv_app_url(cmd_parms *cmd, void *ct, char *arg)
{
	void *cfgAppUrl = NULL;

	//Get a pointer to our server config record...
	uacfg = (union_app_config *) ap_get_module_config(cmd->server->module_config, &union_module);

	//Allocate space for config string in the server heap
	if((cfgAppUrl = ap_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 
			cmd->server,
			"%s: union cfgAppUrl error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgAppUrl, arg);

	//Insert pointer to the cfg path string in the server config record
	uacfg->appUrl = cfgAppUrl;

	return NULL;
}


//设置服务器端模板路径
const char *set_union_serv_tpl_path(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgTplPath = NULL;

	//Get a pointer to our server config record...
	uacfg = (union_app_config *) ap_get_module_config(cmd->server->module_config, &union_module);

	//Allocate space for config string in the server heap
	if((cfgTplPath = ap_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 
			cmd->server,
			"%s: union cfgTplPath error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgTplPath, arg);

	//Insert pointer to the cfg path string in the server config record
	uacfg->appTplPath = cfgTplPath;

	return NULL;
}


//设置服务器端cookie域
const char *set_union_serv_cookie_domain(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgCookieDomain = NULL;

	//Get a pointer to our server config record...
	uacfg = (union_app_config *) ap_get_module_config(cmd->server->module_config, &union_module);

	//Allocate space for config string in the server heap
	if((cfgCookieDomain = ap_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 
			cmd->server,
			"%s: union cfgCookieDomain error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgCookieDomain, arg);

	//Insert pointer to the cfg path string in the server config record
	uacfg->appCookieDomain = cfgCookieDomain;

	return NULL;
}


//设置服务器端cookie名
const char *set_union_serv_cookie_name(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgCookieName = NULL;

	//Get a pointer to our server config record...
	uacfg = (union_app_config *) ap_get_module_config(cmd->server->module_config, &union_module);

	//Allocate space for config string in the server heap
	if((cfgCookieName = ap_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 
			cmd->server,
			"%s: union cfgCookieName error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgCookieName, arg);

	//Insert pointer to the cfg path string in the server config record
	uacfg->appCookieName = cfgCookieName;

	return NULL;
}


/////////////////database config

//设置服务器端db server
const char *set_union_serv_db_serv(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgDbServ = NULL;

	//Get a pointer to our server config record...
	uacfg = (union_app_config *) ap_get_module_config(cmd->server->module_config, &union_module);

	//Allocate space for config string in the server heap
	if((cfgDbServ = ap_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 
			cmd->server,
			"%s: union cfgDbServ error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgDbServ, arg);

	//Insert pointer to the cfg path string in the server config record
	uacfg->appDbServ = cfgDbServ;

	return NULL;
}



//设置服务器端db port
const char *set_union_serv_db_port(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgDbPort = NULL;

	//Get a pointer to our server config record...
	uacfg = (union_app_config *) ap_get_module_config(cmd->server->module_config, &union_module);

	//Allocate space for config string in the server heap
	if((cfgDbPort = ap_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 
			cmd->server,
			"%s: union cfgDbPort error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgDbPort, arg);

	//Insert pointer to the cfg path string in the server config record
	uacfg->appDbPort = cfgDbPort;

	return NULL;
}


//设置服务器端db sock
const char *set_union_serv_db_sock(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgDbSock = NULL;

	//Get a pointer to our server config record...
	uacfg = (union_app_config *) ap_get_module_config(cmd->server->module_config, &union_module);

	//Allocate space for config string in the server heap
	if((cfgDbSock = ap_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 
			cmd->server,
			"%s: union cfgDbSock error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgDbSock, arg);

	//Insert pointer to the cfg path string in the server config record
	uacfg->appDbSock = cfgDbSock;

	return NULL;
}


//设置服务器端db user
const char *set_union_serv_db_user(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgDbUser = NULL;

	//Get a pointer to our server config record...
	uacfg = (union_app_config *) ap_get_module_config(cmd->server->module_config, &union_module);

	//Allocate space for config string in the server heap
	if((cfgDbUser = ap_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 
			cmd->server,
			"%s: union cfgDbUser error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgDbUser, arg);

	//Insert pointer to the cfg path string in the server config record
	uacfg->appDbUser = cfgDbUser;

	return NULL;
}



//设置服务器端db passwd
const char *set_union_serv_db_passwd(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgDbPasswd = NULL;

	//Get a pointer to our server config record...
	uacfg = (union_app_config *) ap_get_module_config(cmd->server->module_config, &union_module);

	//Allocate space for config string in the server heap
	if((cfgDbPasswd = ap_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 
			cmd->server,
			"%s: union cfgDbPasswd error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgDbPasswd, arg);

	//Insert pointer to the cfg path string in the server config record
	uacfg->appDbPwd = cfgDbPasswd;

	return NULL;
}



//设置服务器端db name
const char *set_union_serv_db_name(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgDbName = NULL;

	//Get a pointer to our server config record...
	uacfg = (union_app_config *) ap_get_module_config(cmd->server->module_config, &union_module);

	//Allocate space for config string in the server heap
	if((cfgDbName = ap_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 
			cmd->server,
			"%s: union cfgDbName error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgDbName, arg);

	//Insert pointer to the cfg path string in the server config record
	uacfg->appDbName = cfgDbName;

	return NULL;
}


//db setup config init
void union_db_cfg_init(union_app_config* scfg, db_cfg* cfg)
{
	//init db config
	sprintf(cfg->db_server, "%s", scfg->appDbServ);
	sprintf(cfg->db_user, "%s", scfg->appDbUser);
	sprintf(cfg->db_pwd, "%s", scfg->appDbPwd);
	sprintf(cfg->db_name, "%s", scfg->appDbName);
	cfg->db_port = atoi(scfg->appDbPort);
}

