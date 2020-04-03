/*
* Copyright (c) 2006
* All rights reserved.
* 
* file name��api_util.h
* file desc��common func list define
* author:AndyZhou<diudiu8848@163.com>
*/

#ifndef _API_UTIL_H
#define _API_UTIL_H

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

#ifdef __cplusplus
extern "C" {
#endif

//�ж�һ��˫�ֽ��ַ��Ƿ���GBK���뺺��
int is_chinese(const unsigned char ch1, const unsigned char ch2);

//�ִ���ȡ��֧������, �ɹ��󷵻ط���Ҫ����ִ���
int getSubStr(request_rec* r, char** target, 
			  const char* source, int start, int length);

//�ַ�(��)�滻, tokstr:���滻���ִ� word:�滻���ִ�
void replaceStr(request_rec* r, const char* source_str, char** target_str, 
				char* tokstr, char* word);

//������ֵ����������
//65-90 A-Z
//97-122 a-z 
int mtRand(int begin_num, int end_num);
	
//�ж�ĳ��ֵ�Ƿ���ĳ������Ԫ����, Ԫ������ true:1 false:0
int isInArray(int num, int tmpArray[], int enumNum);

//��������ִ�, rand_str_length:�ִ�����
char* genRandString(int rand_str_length);

//����hash������hash·��  hash_max_num:hashĿ¼���ֵ������Ϊ����
char* computeHashPath(char* hash_str, int hash_max_num);


//��ָ���ָ���ֽ��ִ���array_header����������
//����ƥ��Ľ����
int splitStr(request_rec* r,  char* source_str, 
			 const char tokchar, apr_array_header_t** arrList);


//������鵥��Ԫ��ֵ
char* getArrEnum(apr_array_header_t* arrList, int no);


//URL����
char* urlEscape(char* buffer, int blen, const char* url);

//�ַ��Ƴ�
//1:ok 0:fail
int charRemove(char* buffer, int blen, 
			   const char* src_str, char sig_char);

//ȥ���ո�
char* stringTrim(char str[]);

//�ж��ִ��Ƿ�Ϊ����
//1:ok 0:fail
int isNumeric(char str[]);


//��ȡ�����ļ�����������Ϣģ��
char* getIniValue(char* iniFileContent, char* sectName, char* parameter);

#ifdef __cplusplus
}
#endif



#endif	/* !_API_UTIL_H */
