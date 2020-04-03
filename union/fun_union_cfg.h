/*
 * union server config opt func
 */

#ifndef FUN_UNION_CFG_H
#define FUN_UNION_CFG_H

#include "api_mysql.h"

#define GLOBAL_SECTION "global" //ͨ�����ÿ�
#define DATABASE_SECTION "database" //���ݿ����ÿ�
#define UPLOAD_FILE_TMP_DIR "/tmp" //�ļ��ϴ���ʱĿ¼

//�û�hash�����ֵ
#define UNION_HASH_TAB_MAX 3

//�������ݿ����Ӿ��ʵ��, ���ֳ�����״̬
#define UNION_MYSQL_SERVICE_NUM 3 //����Ϊ����

//��DB MYSQL�������
mysql_handle global_union_handler[UNION_MYSQL_SERVICE_NUM];


/*
//unionӦ�����ýṹ
typedef struct union_app_config {
	char *appName;//Ӧ����
	char *appUrl;//Ӧ��URL
	char *appTplPath;//ģ��·��
	char *appCookieDomain;//cookie����domain
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


//���÷�������Ӧ����
const char *set_union_serv_app_name(cmd_parms *cmd, void *ct, char *arg);

//���÷�������Ӧ��URL
const char *set_union_serv_app_url(cmd_parms *cmd, void *ct, char *arg);

//���÷�������ģ��·��
const char *set_union_serv_tpl_path(cmd_parms *cmd, void *ct, char *arg);

//���÷�������cookie��
const char *set_union_serv_cookie_domain(cmd_parms *cmd, void *ct, char *arg);

//���÷�������cookie��
const char *set_union_serv_cookie_name(cmd_parms *cmd, void *ct, char *arg);


//�������ݿ�dbserver
const char *set_union_serv_db_serv(cmd_parms *cmd, void *ct, char *arg);

//�������ݿ�dbport
const char *set_union_serv_db_port(cmd_parms *cmd, void *ct, char *arg);

//�������ݿ�dbsock
const char *set_union_serv_db_sock(cmd_parms *cmd, void *ct, char *arg);

//�������ݿ�dbuser
const char *set_union_serv_db_user(cmd_parms *cmd, void *ct, char *arg);

//�������ݿ�dbpasswd
const char *set_union_serv_db_passwd(cmd_parms *cmd, void *ct, char *arg);

//�������ݿ�dbname
const char *set_union_serv_db_name(cmd_parms *cmd, void *ct, char *arg);

////////////////////////////////////

//�������ô�������
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

