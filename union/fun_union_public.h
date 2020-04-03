#ifndef FUN_UNION_PUBLIC_H
#define FUN_UNION_PUBLIC_H

#include "fun_cfg.h"


//�û�������Ϣ�ṹ
typedef struct {
	char	uid[10];//�û�id
	char	ename[20];//�û�ename
	char	nick[32];//�û��س�
	char	uhash[10];//�û�hash��
} unionBasicInfo;


//unionӦ��httpd.conf���ýṹ����
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

