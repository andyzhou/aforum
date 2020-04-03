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

//���ǰ��������GET����//////////////////////////
int getSpecGetData(request_rec* r, apr_table_t** _param_list);


//�����������ݲ���ָ������
int signalDataGetData(request_rec* r, char* _param_str,
					  char** data_val, apr_table_t** data_param_list,
					  int check_data_val);


//��������GET��POST����,��ƥ�����ݱ��浽table��(private)
int analyseData(request_rec* r, char* _get_post_data, apr_table_t** _param_list);

//��ʼ������GET����(public)
int initGetPostData(request_rec* r, apr_table_t** _param_list);

#ifdef __cplusplus
}
#endif

#endif	/* !_API_CGI_H */
