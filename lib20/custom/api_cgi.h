#ifndef _API_CGI_H
#define _API_CGI_H

#ifdef __cplusplus
extern "C" {
#endif

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

//获得前端特例化GET数据//////////////////////////
int getSpecGetData(request_rec* r, apr_table_t** _param_list);


//分析最终数据操作指令及其参数
int signalDataGetData(request_rec* r, char* _param_str,
					  char** data_val, apr_table_t** data_param_list,
					  int check_data_val);


//分析常规GET、POST数据,将匹配数据保存到table中(private)
int analyseData(request_rec* r, char* _get_post_data, apr_table_t** _param_list);

//初始化分析GET数据(public)
int initGetPostData(request_rec* r, apr_table_t** _param_list);

#ifdef __cplusplus
}
#endif

#endif	/* !_API_CGI_H */
