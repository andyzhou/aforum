#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "http_request.h"
#include "http_protocol.h"
#include "util_filter.h"

#include "apr_buckets.h"
#include "apr_strings.h"
#include "apr_general.h"
#include "apr_lib.h"
#include "apr_compat.h"
#include "apr_pools.h"

#include "tpllib.h"
#include "fun_cfg.h"
#include "fun_public.h"
#include "apache_request.h"


//load apply interface
#include "fun_union_interface.h"
#include "fun_forum_public.h"
//#include "fun_forum_search.h"
#include "fun_forum_interface.h"
#include "fun_sys_interface.h"

#define AFORUM_MAIN_HANDLER "aforum-handler"

static apr_global_mutex_t *client_lock = NULL;

//declear module
module AP_MODULE_DECLARE_DATA aforum_module;

//handler
static int aforum_handler(request_rec* r)
{
	//filter suit handler
	if(strcmp(r->handler, AFORUM_MAIN_HANDLER))
	{
		return DECLINED;
	}

	/*
	//////////// test begin
	ap_set_content_type(r, "text/html");
	ap_rputs("<h1>aforum test 123</h1>",r );
	ap_rprintf(r, "<h1>aForumRunUrl:%s</h1>", acfg->appRunUrl);
	return OK;
	//////////// test end
	*/


	apr_table_t* param_list;
	ApacheRequest *req;
	const char* func = 0;
	int param_num = 0;

	//get current user cookie info
	get_general_cookie(r, acfg, &ucocfg);

	
	//distribute every cmd, by GET or POST method
	switch(r->method_number)
	{
	case M_POST://m_post
		{
			req = ApacheRequest_new(r);
			req->temp_dir = UPLOAD_FILE_TMP_DIR;
			func = ApacheRequest_param(req, "func");//get sub function

			/*
			ApacheUpload *upload_files;
			ap_set_content_type(r, "text/html");

			for(upload_files = ApacheRequest_upload(req); upload_files;
				upload_files = upload_files->next)
			{
					ap_rprintf(r, "<h1>file:%s</h1>", upload_files->tempname);
					ap_rprintf(r, "<h1>tmpSize:%d</h1>", atol(upload_files->tmpSize));
			}

			return OK;
			*/

			if(func && !strcmp(func, "union"))
			{
				//union post interface
				union_post_method(r, acfg, req);
				return OK;
			}
			else if(func && !strcmp(func, "sys"))
			{
				//sys post interface
				sys_post_method(r, acfg, req);
				return OK;
			}
			else
			{
				//forum post interface
				forum_post_method(r, acfg, req);
				return OK;
			}
		}
		break;

	default://m_get
		{
			param_num = getSpecGetData(r, &param_list);
			func = (char*)ap_table_get(param_list, "1");

			if(func && !strcmp(func, "union"))
			{
				//union interface
				union_get_method(r, acfg, param_list);
				return OK;
			}
			else if(func && !strcmp(func, "sys"))
			{
				//sys interface
				sys_get_method(r, acfg, param_list);
				return OK;
			}
			/*
			else if(func && !strcmp(func, "search"))
			{
				//search interface
				topic_search_form(r, acfg);
				return OK;
			}
			*/
			else
			{
				//forum interface
				forum_get_method(r, acfg, param_list);
			}
		}
		break;
	}

	return OK;


	/*
	//测试apreq
	ApacheUpload *upload_files;
	ApacheRequest *req = ApacheRequest_new(r);
	const char* key = 0;
	int file_no = 0;

	key = ApacheRequest_param(req, "key");
	ap_rprintf(r, "<h1>key:%s</h1>", key);

	//设置临时文件存放路径
	req->temp_dir = "/tmp";

	for(upload_files = ApacheRequest_upload(req), file_no = 1; upload_files;
		upload_files = upload_files->next, file_no++)
	{
			ap_rprintf(r, "<h1>file:%s</h1>", upload_files->tempname);
	}

	return OK;



	////////////////////////////

	//测试模板
	tpl_t *tpl = tpl_alloc();
	int rows = 10, i = 0;
	char *content;
	char *tpl_file = "/usr/local/apache2/htdocs/test.tpl";

	// Load template
    if (tpl_load(r, tpl, tpl_file) != TPL_OK)
    {
        ap_rprintf(r, "<h1>can not load tpl file</h1>");
		return OK;
    }

	tpl_set_field_int_global(tpl, "ROWS", rows);
	tpl_set_field_fmt_global(tpl, "HELLO", "%s", "diudiu");


	//sections
	tpl_select_section(tpl, "ROW");
        
    for (i = 1; i <= rows; i++)
    	{
        int j = (i - 1) * 10;
        
        tpl_set_field_int(tpl, "TYPE", i % 2);
            
        tpl_set_field_fmt(tpl, "FIELD1", "Row %d", i);
        tpl_set_field_int(tpl, "FIELD2", j + 2);
        
        tpl_append_section(tpl);
    	}
    tpl_deselect_section(tpl);

	content = malloc(tpl_length(tpl) + 1);

    tpl_get_content(tpl, content);

	ap_rprintf(r, "%s", content);

    tpl_free(tpl);


	return OK;
	*/
}


//child init
static void aforum_child_init(apr_pool_t* p, server_rec* s)
{
	apr_status_t sts;
	int i = 0, ret = 0;
	char *errBuf = 0;

	ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
                 "Aforum child process init.........");

	//init db config
	af_db_cfg_init(acfg, &af_db_cfg);

	//create mysql connect
	for(i = 0; i < MYSQL_SERVICE_NUM; i++)
	{
		if(!global_af_handler[i].isConnect || !global_af_handler[i].pConn)
		{
			ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
						 "Aforum child process init db connect.......");

			//connect mysql server
			ret = mysqlSimpConnect(p, af_db_cfg, &global_af_handler[i], &errBuf);

			if(!ret)
			{
				//failed
				ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
							 "failed .. %s", errBuf);
			}
			else
			{
				//success
				ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, 
							 "success!");
			}

			usleep(5000);
		}
	}
}


//registe hooks
static void af_registe_hooks(apr_pool_t* p)
{
	//child init
	ap_hook_child_init(aforum_child_init, NULL, NULL, APR_HOOK_MIDDLE);

	//handler
	ap_hook_handler(aforum_handler, NULL, NULL, APR_HOOK_MIDDLE);
}


module AP_MODULE_DECLARE_DATA aforum_module = {
		STANDARD20_MODULE_STUFF,
		NULL,//create dir config
		NULL,//merge dir config

		create_af_server_config,//create server config
		merege_af_server_config,//merge server config

		af_cmds,//cmds
		af_registe_hooks
};
