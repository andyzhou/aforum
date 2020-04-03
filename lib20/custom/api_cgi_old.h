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
#include "ap_alloc.h"

//获得前端特例化GET数据//////////////////////////
int getSpecGetData(request_rec* r, table** _param_list);

//获得单个列数据
char* getSpecGetVal(array_header* _param_list, int _col_num);

//分析最终数据操作指令及其参数
int signalDataGetData(request_rec* r, pool* p, char* _param_str, char** data_val,
					  table** data_param_list);

/////////////////////////////////////////////////

//分析常规GET、POST数据,将匹配数据保存到table中
int analyseData(request_rec* r, char* _get_post_data, table** _param_list);

//初始化分析GET数据
int initGetData(request_rec* r, table** _param_list);

#ifdef __cplusplus
}
#endif

#endif	/* !_API_CGI_H */
