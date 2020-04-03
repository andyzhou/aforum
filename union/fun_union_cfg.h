/*
 * union server config opt func
 */

#ifndef FUN_UNION_CFG_H
#define FUN_UNION_CFG_H

#include "api_mysql.h"

#define GLOBAL_SECTION "global" //通用配置块
#define DATABASE_SECTION "database" //数据库配置块
#define UPLOAD_FILE_TMP_DIR "/tmp" //文件上传临时目录

//用户hash表最大值
#define UNION_HASH_TAB_MAX 3

//创建数据库连接句柄实例, 保持长连接状态
#define UNION_MYSQL_SERVICE_NUM 3 //必须为基数

//主DB MYSQL操作句柄
mysql_handle global_union_handler[UNION_MYSQL_SERVICE_NUM];


/*
//union应用配置结构
typedef struct union_app_config {
	char *appName;//应用名
	char *appUrl;//应用URL
	char *appTplPath;//模板路径
	char *appCookieDomain;//cookie设置domain
	char *appCookieName;//cookie Name

	char *appDbServ;//db server
	char *appDbPort;//db port
	char *appDbSock;//db socket file
	char *appDbUser;//db user
	char *appDbPwd;//db password
	char *appDbName;//db name

} union_app_config;


union_app_config *uacfg;
*/

//db config variable
db_cfg union_db_cfg;

//create union server config
void *create_union_server_config(pool *p, server_rec *s);


//merge union server config
void *merege_union_server_config(pool *p, void *parent, void *sub);

////////////////////////////////////////////////////////


//设置服务器端应用名
const char *set_union_serv_app_name(cmd_parms *cmd, void *ct, char *arg);

//设置服务器端应用URL
const char *set_union_serv_app_url(cmd_parms *cmd, void *ct, char *arg);

//设置服务器端模板路径
const char *set_union_serv_tpl_path(cmd_parms *cmd, void *ct, char *arg);

//设置服务器端cookie域
const char *set_union_serv_cookie_domain(cmd_parms *cmd, void *ct, char *arg);

//设置服务器端cookie名
const char *set_union_serv_cookie_name(cmd_parms *cmd, void *ct, char *arg);


//设置数据库dbserver
const char *set_union_serv_db_serv(cmd_parms *cmd, void *ct, char *arg);

//设置数据库dbport
const char *set_union_serv_db_port(cmd_parms *cmd, void *ct, char *arg);

//设置数据库dbsock
const char *set_union_serv_db_sock(cmd_parms *cmd, void *ct, char *arg);

//设置数据库dbuser
const char *set_union_serv_db_user(cmd_parms *cmd, void *ct, char *arg);

//设置数据库dbpasswd
const char *set_union_serv_db_passwd(cmd_parms *cmd, void *ct, char *arg);

//设置数据库dbname
const char *set_union_serv_db_name(cmd_parms *cmd, void *ct, char *arg);

////////////////////////////////////

//设置配置处理命令
static const command_rec unionServ_cmds[] = {

	//general config
	{"UnionAppName", set_union_serv_app_name, NULL, RSRC_CONF, TAKE1,
    "Can not find union app name."},
	
	{"UnionAppUrl", set_union_serv_app_url, NULL, RSRC_CONF, TAKE1,
    "Can not find union app url."},
	
	{"UnionTplPath", set_union_serv_tpl_path, NULL, RSRC_CONF, TAKE1,
    "Can not find union tpl path."},
	
	{"UnionCookieDomain", set_union_serv_cookie_domain, NULL, RSRC_CONF, TAKE1,
    "Can not find union cookie domain."},
	
	{"UnionCookieName", set_union_serv_cookie_name, NULL, RSRC_CONF, TAKE1,
    "Can not find union cookie name."},

	//database config	
	{"UnionDbServ", set_union_serv_db_serv, NULL, RSRC_CONF, TAKE1,
    "Can not find union db server."},
	
	{"UnionDbPort", set_union_serv_db_port, NULL, RSRC_CONF, TAKE1,
    "Can not find union db port."},
	
	{"UnionDbSock", set_union_serv_db_sock, NULL, RSRC_CONF, TAKE1,
    "Can not find union db sock."},
	
	{"UnionDbUser", set_union_serv_db_user, NULL, RSRC_CONF, TAKE1,
    "Can not find union db user."},
	
	{"UnionDbPasswd", set_union_serv_db_passwd, NULL, RSRC_CONF, TAKE1,
    "Can not find union db password."},
	
	{"UnionDbName", set_union_serv_db_name, NULL, RSRC_CONF, TAKE1,
    "Can not find union db name."},

    {NULL}
};

//db setup config init
void union_db_cfg_init(union_app_config* scfg, db_cfg* cfg);

#endif

