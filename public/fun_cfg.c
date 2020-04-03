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
#include "ap_compat.h"
#include "apr_pools.h"

#include "fun_cfg.h"


//extend aforum module variable
extern module AP_MODULE_DECLARE_DATA aforum_module;


//create forum server config
void* create_af_server_config(apr_pool_t *p, server_rec *s)
{
	
	af_app_config *rec = (af_app_config *)apr_palloc(p, sizeof(af_app_config));

	//set default value
	rec->appName	= "af";
	rec->appUrl		= "localhost";
	rec->appRunUrl	= "/af";
	rec->appTplPath = "./tpl";
	rec->appUploadPath = "/tmp";
	rec->appUploadUrl = "/upload";

	rec->appCookieDomain = "";
	rec->appCookieName = "af";

	rec->appCharSet = "utf-8";

	rec->appRecPePage = "20";
	rec->appRecSubPage = "10";
	
	rec->appDbServ = "localhost";
	rec->appDbPort = "3306";
	rec->appDbSock = "/tmp/mysql.sock";
	rec->appDbUser = "root";
	rec->appDbPwd  = "";
	rec->appDbName = "af";

	return (void *)rec;
}


//merge forum server config
void* merege_af_server_config(apr_pool_t *p, void *parent, void *sub)
{
	af_app_config *par  = (af_app_config *) parent;
    af_app_config *chld = (af_app_config *) sub;
    af_app_config *mrg;
	
	mrg = (af_app_config *) apr_palloc(p, sizeof(af_app_config));

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

	if(chld->appRunUrl)
	{
		mrg->appRunUrl = par->appRunUrl;
	}
	else
	{
		mrg->appRunUrl = chld->appRunUrl;
	}
	
	if(chld->appTplPath)
	{
		mrg->appTplPath = par->appTplPath;
	}
	else
	{
		mrg->appTplPath = chld->appTplPath;
	}
	
	
	if(chld->appUploadPath)
	{
		mrg->appUploadPath = par->appUploadPath;
	}
	else
	{
		mrg->appUploadPath = chld->appUploadPath;
	}
	
	
	if(chld->appUploadUrl)
	{
		mrg->appUploadUrl = par->appUploadUrl;
	}
	else
	{
		mrg->appUploadUrl = chld->appUploadUrl;
	}

	/////////////////////////
	
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

	
	if(chld->appCharSet)
	{
		mrg->appCharSet = par->appCharSet;
	}
	else
	{
		mrg->appCharSet = chld->appCharSet;
	}

	/////////////////////


	if(chld->appRecPePage)
	{
		mrg->appRecPePage = par->appRecPePage;
	}
	else
	{
		mrg->appRecPePage = chld->appRecPePage;
	}

	if(chld->appRecSubPage)
	{
		mrg->appRecSubPage = par->appRecSubPage;
	}
	else
	{
		mrg->appRecSubPage = chld->appRecSubPage;
	}

	///////////////////


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



//set config of app name
const char* set_af_serv_app_name(cmd_parms *cmd, void *ct, char *arg)
{
	void *cfgAppName = NULL;

	//Get a pointer to our server config record...
	acfg = (af_app_config *) ap_get_module_config(cmd->server->module_config, &aforum_module);

	//Allocate space for config string in the server heap
	if((cfgAppName = apr_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 0, 
			cmd->server,
			"%s: aForumAppName error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgAppName, arg);

	//Insert pointer to the cfg path string in the server config record
	acfg->appName = cfgAppName;

	return NULL;
}


//set config of app url
const char* set_af_serv_app_url(cmd_parms *cmd, void *ct, char *arg)
{
	void *cfgAppUrl = NULL;

	//Get a pointer to our server config record...
	acfg = (af_app_config *) ap_get_module_config(cmd->server->module_config, &aforum_module);

	//Allocate space for config string in the server heap
	if((cfgAppUrl = apr_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 0,
			cmd->server,
			"%s: aForumAppUrl error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgAppUrl, arg);

	//Insert pointer to the cfg path string in the server config record
	acfg->appUrl = cfgAppUrl;

	return NULL;
}


//set config of app run url
const char* set_af_serv_run_url(cmd_parms *cmd, void *ct, char *arg)
{
	void *cfgRunUrl = NULL;

	//Get a pointer to our server config record...
	acfg =(af_app_config *) ap_get_module_config(cmd->server->module_config, &aforum_module);

	//Allocate space for config string in the server heap
	if((cfgRunUrl = apr_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 0, 
			cmd->server,
			"%s: aForumRunUrl error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgRunUrl, arg);

	//Insert pointer to the cfg path string in the server config record
	acfg->appRunUrl = cfgRunUrl;

	return NULL;
}


//set config of tpl path
const char* set_af_serv_tpl_path(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgTplPath = NULL;

	//Get a pointer to our server config record...
	acfg = (af_app_config *) ap_get_module_config(cmd->server->module_config, &aforum_module);

	//Allocate space for config string in the server heap
	if((cfgTplPath = apr_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 0,
			cmd->server,
			"%s: aForumTplPath error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgTplPath, arg);

	//Insert pointer to the cfg path string in the server config record
	acfg->appTplPath = cfgTplPath;

	return NULL;
}


//set config of attach path
const char* set_af_serv_upload_path(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgUploadPath = NULL;

	//Get a pointer to our server config record...
	acfg = (af_app_config *) ap_get_module_config(cmd->server->module_config, &aforum_module);

	//Allocate space for config string in the server heap
	if((cfgUploadPath = apr_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 0,
			cmd->server,
			"%s: aForumUploadPath error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgUploadPath, arg);

	//Insert pointer to the cfg path string in the server config record
	acfg->appUploadPath = cfgUploadPath;

	return NULL;
}



//set config of attach url
const char* set_af_serv_upload_url(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgUploadUrl = NULL;

	//Get a pointer to our server config record...
	acfg = (af_app_config *) ap_get_module_config(cmd->server->module_config, &aforum_module);

	//Allocate space for config string in the server heap
	if((cfgUploadUrl = apr_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 0,
			cmd->server,
			"%s: aForumUploadUrl error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgUploadUrl, arg);

	//Insert pointer to the cfg path string in the server config record
	acfg->appUploadUrl = cfgUploadUrl;

	return NULL;
}



//set config of cookie domain
const char* set_af_serv_cookie_domain(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgCookieDomain = NULL;

	//Get a pointer to our server config record...
	acfg = (af_app_config *) ap_get_module_config(cmd->server->module_config, &aforum_module);

	//Allocate space for config string in the server heap
	if((cfgCookieDomain = apr_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 0, 
			cmd->server,
			"%s: aForumCookieDomain error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgCookieDomain, arg);

	//Insert pointer to the cfg path string in the server config record
	acfg->appCookieDomain = cfgCookieDomain;

	return NULL;
}


//set config of cookie name
const char* set_af_serv_cookie_name(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgCookieName = NULL;

	//Get a pointer to our server config record...
	acfg = (af_app_config *) ap_get_module_config(cmd->server->module_config, &aforum_module);

	//Allocate space for config string in the server heap
	if((cfgCookieName = apr_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 0,
			cmd->server,
			"%s: aForumCookieName error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgCookieName, arg);

	//Insert pointer to the cfg path string in the server config record
	acfg->appCookieName = cfgCookieName;

	return NULL;
}


//set page character set type
const char* set_af_serv_character_set(cmd_parms *cmd, void *ct, char *arg)
{
	void *cfgCharSet = NULL;

	//Get a pointer to our server config record...
	acfg = (af_app_config *) ap_get_module_config(cmd->server->module_config, &aforum_module);

	//Allocate space for config string in the server heap
	if((cfgCharSet = apr_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 0,
			cmd->server,
			"%s: aForumCharSet error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgCharSet, arg);

	//Insert pointer to the cfg path string in the server config record
	acfg->appCharSet = cfgCharSet;

	return NULL;
}


//set config of records per page
const char* set_af_serv_rec_per_page(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgRecPePage = NULL;

	//Get a pointer to our server config record...
	acfg = (af_app_config *) ap_get_module_config(cmd->server->module_config, &aforum_module);

	//Allocate space for config string in the server heap
	if((cfgRecPePage = apr_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{ 0,
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 0,
			cmd->server,
			"%s: aForumRecPePage error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgRecPePage, arg);

	//Insert pointer to the cfg path string in the server config record
	acfg->appRecPePage = cfgRecPePage;

	return NULL;
}



//set config of records per sub page
const char* set_af_serv_rec_sub_page(cmd_parms *cmd, void *ct, char *arg)
{
	void *cfgRecSubPage = NULL;

	//Get a pointer to our server config record...
	acfg = (af_app_config *) ap_get_module_config(cmd->server->module_config, &aforum_module);

	//Allocate space for config string in the server heap
	if((cfgRecSubPage = apr_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 0, 
			cmd->server,
			"%s: aForumRecSubPage error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgRecSubPage, arg);

	//Insert pointer to the cfg path string in the server config record
	acfg->appRecSubPage = cfgRecSubPage;

	return NULL;
}


/////////////////database config

//set config of db server 
const char* set_af_serv_db_serv(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgDbServ = NULL;

	//Get a pointer to our server config record...
	acfg = (af_app_config *) ap_get_module_config(cmd->server->module_config, &aforum_module);

	//Allocate space for config string in the server heap
	if((cfgDbServ = apr_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 0, 
			cmd->server,
			"%s: aForumDbServ error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgDbServ, arg);

	//Insert pointer to the cfg path string in the server config record
	acfg->appDbServ = cfgDbServ;

	return NULL;
}



//set config of db server port
const char* set_af_serv_db_port(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgDbPort = NULL;

	//Get a pointer to our server config record...
	acfg = (af_app_config *) ap_get_module_config(cmd->server->module_config, &aforum_module);

	//Allocate space for config string in the server heap
	if((cfgDbPort = apr_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 0, 
			cmd->server,
			"%s: aForumDbPort error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgDbPort, arg);

	//Insert pointer to the cfg path string in the server config record
	acfg->appDbPort = cfgDbPort;

	return NULL;
}


//set db sock
const char* set_af_serv_db_sock(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgDbSock = NULL;

	//Get a pointer to our server config record...
	acfg = (af_app_config *) ap_get_module_config(cmd->server->module_config, &aforum_module);

	//Allocate space for config string in the server heap
	if((cfgDbSock = apr_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 0, 
			cmd->server,
			"%s: aForumDbSock error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgDbSock, arg);

	//Insert pointer to the cfg path string in the server config record
	acfg->appDbSock = cfgDbSock;

	return NULL;
}


//set config of db user
const char* set_af_serv_db_user(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgDbUser = NULL;

	//Get a pointer to our server config record...
	acfg = (af_app_config *) ap_get_module_config(cmd->server->module_config, &aforum_module);

	//Allocate space for config string in the server heap
	if((cfgDbUser = apr_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 0, 
			cmd->server,
			"%s: aForumDbUser error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgDbUser, arg);

	//Insert pointer to the cfg path string in the server config record
	acfg->appDbUser = cfgDbUser;

	return NULL;
}



//set config of db password
const char* set_af_serv_db_passwd(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgDbPasswd = NULL;

	//Get a pointer to our server config record...
	acfg = (af_app_config *) ap_get_module_config(cmd->server->module_config, &aforum_module);

	//Allocate space for config string in the server heap
	if((cfgDbPasswd = apr_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 0, 
			cmd->server,
			"%s: aForumDbPasswd error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgDbPasswd, arg);

	//Insert pointer to the cfg path string in the server config record
	acfg->appDbPwd = cfgDbPasswd;

	return NULL;
}



//set config of db name
const char* set_af_serv_db_name(cmd_parms *cmd, void *ct, char *arg)
{
	
	void *cfgDbName = NULL;

	//Get a pointer to our server config record...
	acfg = (af_app_config *) ap_get_module_config(cmd->server->module_config, &aforum_module);

	//Allocate space for config string in the server heap
	if((cfgDbName = apr_pcalloc(cmd->pool, (strlen(arg) + 1) * sizeof(char))) == NULL)
	{
		ap_log_error(APLOG_MARK, 
			APLOG_NOERRNO | APLOG_NOTICE, 0, 
			cmd->server,
			"%s: aForumDbName error: Apache would not allocate %d bytes of memory to store cfg path string.",
			__FILE__, 
			(strlen(arg) + 1));

		return NULL;
	}

	//copy data to cfg
	sprintf(cfgDbName, arg);

	//Insert pointer to the cfg path string in the server config record
	acfg->appDbName = cfgDbName;

	return NULL;
}


