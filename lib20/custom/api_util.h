/*
* Copyright (c) 2006
* All rights reserved.
* 
* file name：api_util.h
* file desc：common func list define
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

//判断一个双字节字符是否是GBK编码汉字
int is_chinese(const unsigned char ch1, const unsigned char ch2);

//字串截取，支持中文, 成功后返回符号要求的字串长
int getSubStr(request_rec* r, char** target, 
			  const char* source, int start, int length);

//字符(串)替换, tokstr:需替换的字串 word:替换的字串
void replaceStr(request_rec* r, const char* source_str, char** target_str, 
				char* tokstr, char* word);

//产生定值区间的随机数
//65-90 A-Z
//97-122 a-z 
int mtRand(int begin_num, int end_num);
	
//判断某个值是否在某个数组元素中, 元素总数 true:1 false:0
int isInArray(int num, int tmpArray[], int enumNum);

//产生随机字串, rand_str_length:字串长度
char* genRandString(int rand_str_length);

//根据hash串计算hash路径  hash_max_num:hash目录最大值，必须为奇数
char* computeHashPath(char* hash_str, int hash_max_num);


//按指定分割符分解字串到array_header类型数组中
//返回匹配的结果数
int splitStr(request_rec* r,  char* source_str, 
			 const char tokchar, apr_array_header_t** arrList);


//获得数组单个元素值
char* getArrEnum(apr_array_header_t* arrList, int no);


//URL编码
char* urlEscape(char* buffer, int blen, const char* url);

//字符移除
//1:ok 0:fail
int charRemove(char* buffer, int blen, 
			   const char* src_str, char sig_char);

//去除空格
char* stringTrim(char str[]);

//判断字串是否为数字
//1:ok 0:fail
int isNumeric(char str[]);


//读取配置文件各个配置信息模块
char* getIniValue(char* iniFileContent, char* sectName, char* parameter);

#ifdef __cplusplus
}
#endif



#endif	/* !_API_UTIL_H */
