#ifndef FUN_CFG_H
#define FUN_CFG_H

typedef struct union_app_config {
	char *appName;//app name
	char *appUrl;//app url
	char *appRunUrl;//module run url
	char *appTplPath;//tpl path
	char *appUploadPath;//attach save path
	char *appUploadUrl;//attach url

	char *appCookieDomain;//cookie domain
	char *appCookieName;//cookie Name

	char *appCharSet;//page character type

	char *appRecPePage;//record per page
	char *appRecSubPage;//record sub page

	char *appDbServ;//db server
	char *appDbPort;//db port
	char *appDbSock;//db socket file
	char *appDbUser;//db user
	char *appDbPwd;//db password
	char *appDbName;//db name

} af_app_config;

//application global config
af_app_config *acfg;




/////////////////// sub functions define ////////////////
//create forum server config
void* create_af_server_config(apr_pool_t *p, server_rec *s);

//merge forum server config
void* merege_af_server_config(apr_pool_t *p, void *parent, void *sub);



////////////// set server config name and value ////////////////////
//set apply name
const char* set_af_serv_app_name(cmd_parms *cmd, void *ct, char *arg);

//set apply url
const char* set_af_serv_app_url(cmd_parms *cmd, void *ct, char *arg);

//set apply run url
const char* set_af_serv_run_url(cmd_parms *cmd, void *ct, char *arg);

//set tpl path
const char* set_af_serv_tpl_path(cmd_parms *cmd, void *ct, char *arg);

//set attach path
const char* set_af_serv_upload_path(cmd_parms *cmd, void *ct, char *arg);

//set attach url
const char* set_af_serv_upload_url(cmd_parms *cmd, void *ct, char *arg);


//set records per page
const char* set_af_serv_rec_per_page(cmd_parms *cmd, void *ct, char *arg);

//set records per sub page
const char* set_af_serv_rec_sub_page(cmd_parms *cmd, void *ct, char *arg);


//set cookie domain
const char* set_af_serv_cookie_domain(cmd_parms *cmd, void *ct, char *arg);

//set cookie name
const char* set_af_serv_cookie_name(cmd_parms *cmd, void *ct, char *arg);


//set page character set type
const char* set_af_serv_character_set(cmd_parms *cmd, void *ct, char *arg);

//set db server
const char* set_af_serv_db_serv(cmd_parms *cmd, void *ct, char *arg);

//set db port
const char* set_af_serv_db_port(cmd_parms *cmd, void *ct, char *arg);

//set db sock
const char* set_af_serv_db_sock(cmd_parms *cmd, void *ct, char *arg);

//set db user
const char* set_af_serv_db_user(cmd_parms *cmd, void *ct, char *arg);

//set db password
const char* set_af_serv_db_passwd(cmd_parms *cmd, void *ct, char *arg);

//set db name
const char* set_af_serv_db_name(cmd_parms *cmd, void *ct, char *arg);



//set cmds for config
static const command_rec af_cmds[] = {

	//general config
	AP_INIT_TAKE1("aForumAppName", set_af_serv_app_name, NULL, RSRC_CONF, "Can not find aForumAppName."),
	
	AP_INIT_TAKE1("aForumAppUrl", set_af_serv_app_url, NULL, RSRC_CONF, "Can not find aForumAppUrl."),
	
	AP_INIT_TAKE1("aForumTplPath", set_af_serv_tpl_path, NULL, RSRC_CONF, "Can not find aForumTplPath."),

	AP_INIT_TAKE1("aForumUploadPath", set_af_serv_upload_path, NULL, RSRC_CONF, "Can not find aForumAttachPath."),
	
	AP_INIT_TAKE1("aForumUploadUrl", set_af_serv_upload_url, NULL, RSRC_CONF, "Can not find aForumAttachUrl."),
	
	AP_INIT_TAKE1("aForumRunUrl", set_af_serv_run_url, NULL, RSRC_CONF, "Can not find aForumRunUrl."),
	
	AP_INIT_TAKE1("aForumRecPePage", set_af_serv_rec_per_page, NULL, RSRC_CONF, "Can not find aForumRecPePage."),
	
	AP_INIT_TAKE1("aForumRecSubPage", set_af_serv_rec_sub_page, NULL, RSRC_CONF, "Can not find aForumRecSubPage."),
	
	AP_INIT_TAKE1("aForumCookieDomain", set_af_serv_cookie_domain, NULL, RSRC_CONF, "Can not find aForumCookieDomain."),
	
	AP_INIT_TAKE1("aForumCookieName", set_af_serv_cookie_name, NULL, RSRC_CONF, "Can not find aForumCookieName."),

	AP_INIT_TAKE1("aForumCharSet", set_af_serv_character_set, NULL, RSRC_CONF, "Can not find aForumCharSet."),


	//database config	
	AP_INIT_TAKE1("aForumDbServ", set_af_serv_db_serv, NULL, RSRC_CONF, "Can not find aForumDbServ."),
	
	AP_INIT_TAKE1("aForumDbPort", set_af_serv_db_port, NULL, RSRC_CONF, "Can not find aForumDbPort."),
	
	AP_INIT_TAKE1("aForumDbSock", set_af_serv_db_sock, NULL, RSRC_CONF, "Can not find aForumDbSock."),
	
	AP_INIT_TAKE1("aForumDbUser", set_af_serv_db_user, NULL, RSRC_CONF, "Can not find aForumDbUser."),
	
	AP_INIT_TAKE1("aForumDbPasswd", set_af_serv_db_passwd, NULL, RSRC_CONF, "Can not find aForumDbPasswd."),
	
	AP_INIT_TAKE1("aForumDbName", set_af_serv_db_name, NULL, RSRC_CONF, "Can not find aForumDbName."),

    {NULL}
};

#endif

