#ifndef FUN_UNION_PUBLIC_H
#define FUN_UNION_PUBLIC_H

#include "fun_cfg.h"


//用户基本信息结构
typedef struct {
	char	uid[10];//用户id
	char	ename[20];//用户ename
	char	nick[32];//用户呢称
	char	uhash[10];//用户hash表
} unionBasicInfo;


//union应用httpd.conf配置结构变量
//app_config *ucfg;

//////////////////////////////////////

//union generic page header
void output_union_pageHeader(request_rec* r, af_app_config *uacfg,
							 char* title, unionCookieInfo ucocfg);

//union page bottom
void output_union_pageBottom(request_rec* r, af_app_config *uacfg);


//show union error msg page
void show_union_err_msg(request_rec* r, af_app_config *uacfg, 
						char* errMsg, char* returnUrl, int ok_or_failed);




/*
//get signal user node
//ret -1:error param  0:failed 1:ok
int union_get_siguser(request_rec* r, unionGlobalCfg* scfg,
					  char* uemail, sigNodeVal **snv);
*/

#endif

