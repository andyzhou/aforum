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

//���ǰ��������GET����//////////////////////////
int getSpecGetData(request_rec* r, table** _param_list);

//��õ���������
char* getSpecGetVal(array_header* _param_list, int _col_num);

//�����������ݲ���ָ������
int signalDataGetData(request_rec* r, pool* p, char* _param_str, char** data_val,
					  table** data_param_list);

/////////////////////////////////////////////////

//��������GET��POST����,��ƥ�����ݱ��浽table��
int analyseData(request_rec* r, char* _get_post_data, table** _param_list);

//��ʼ������GET����
int initGetData(request_rec* r, table** _param_list);

#ifdef __cplusplus
}
#endif

#endif	/* !_API_CGI_H */
