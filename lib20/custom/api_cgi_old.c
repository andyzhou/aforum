
#include "api_cgi.h"

//中文编码转换
//接口类型：private
char x2c(char *what);

//中文编码转换
//接口类型：private
char* unescape_url(char* url);

//接口类型：private
char* czMemMemI(const char* s1, int n1, const char* s2, int n2);

//分析带有附件的POST数据 multipart/form-data
//接口类型：private
int parse_multi_data(request_rec* r, int nBytes, char* _content_data,
					   table** _param_list);

//获得前端特例化GET数据
//接口类型：public
int getSpecGetData(request_rec* r, table** _param_list)
{
	char* _spec_get_data = 0;
	char* key = 0;
	char* val = 0;
	char key_str[20] = {0};
	int _param_num = 0;

	_spec_get_data = r->unparsed_uri;
	
	*_param_list = ap_make_table(r->pool, 1);

	while (*_spec_get_data != '\0')
	{
		key = ap_getword_nc(r->pool, &_spec_get_data, '/');

		if(!key || !(*key))
		{
			continue;
		}

		memset(key_str, 0, 20);
		sprintf(key_str, "%d", _param_num);

		//将参数键、值加入到参数表
		ap_table_add(*_param_list, key_str, key);

		_param_num++;
	}

	return _param_num;
}


//获得单个列数据
//接口类型：public
char* getSpecGetVal(array_header* _param_list, int _col_num)
{
	char* _suit_val = 0;

	if(!_param_list)
	{
		return _suit_val;
	}

	if(_col_num < 0)
	{
		return _suit_val;
	}
	
	char** list = (char**)_param_list->elts;

	_suit_val = list[_col_num];

	return (!_suit_val || !(*_suit_val)) ? 0 : _suit_val;
}


//分析最终数据操作指令及其参数
int signalDataGetData(request_rec* r, pool* p, char* _param_str, char** data_val,
					  table** data_param_list)
{
	char* key = 0;
	char* val = 0;
	char* _val = 0;

	//分析数据相关参数
	*data_param_list = ap_make_table(p, 1);

	if(!_param_str || !(*_param_str))
	{
		return 0;
	}

	if(!strstr(_param_str, "?"))
	{
		*data_val = _param_str;

		return 1;
	}

	///获得data_val值
	while (*_param_str != '\0')
	{
		_val = ap_getword_nc(r->pool, &_param_str, '?');

		if(_val)
		{
			*data_val = _val;
		}

		break;
	}

	if(!data_val || !(*data_val))
	{
		return 0;
	}

	//ap_rprintf(r, "<h1>data_val:%s</h1>", *data_val);

	if(!_param_str || !(*_param_str))
	{
		return 0;
	}

	while (*_param_str != '\0')
	{
		key = ap_getword_nc(r->pool, &_param_str, '&');

		if(!key || !(*key))
		{
			continue;
		}
		
		if ((val = strchr(key, '=')) != NULL)
		{
			*val++ = '\0';

			//add key and value to table
			//ap_unescape_url(val);
			//将参数键、值加入到参数表
			ap_table_add(*data_param_list, key, val);
			//ap_rprintf(r, "<h1>key:%s, val:%s</h1>", key_str, val);
		}
	}

	return 1;
}


////////////////////////////////

//分析常规GET、POST数据,将匹配数据保存到table中
//接口类型：private
int analyseData(request_rec *r, char* _get_post_data, table** _param_list)
{
	char* key = 0;
	char* val = 0;
	int i = 0;
	
	ap_rputs(_get_post_data, r);
	
	if(!_get_post_data || !(*_get_post_data))
	{
		return 0;
	}
	
	while (*_get_post_data != '\0')
	{
		key = ap_getword_nc(r->pool, &_get_post_data, '&');

		if(!key || !(*key) || !strcmp(key, "(null)"))
		{
			continue;
		}
		
		if ((val = strchr(key, '=')) != NULL)
		{
			*val++ = '\0';

			ap_rprintf(r, "<h1>key:%s, val:%s</h1>", key, val);

			ap_unescape_url(val);
			
			//add key and value to table
			//ap_unescape_url(val);
			//将参数键、值加入到参数表
			ap_table_add(*_param_list, key, val);
			//ap_table_add(*_param_list, key, val);

			i++;
		}
	}
	
	return i;
}

//初始化|分析GET数据
//接口类型：public
int initGetData(request_rec* r, table** _param_list)
{
	char* _get_post_data = 0;
	const char* _post_data_len_str = 0;
	const char* _content_type = 0;
	
	int _post_data_len = 0;
	int _rstat = 0;
	int _nbytes = 0;
	int _param_num = 0;

	//初始化参数存储table
	*_param_list = ap_make_table(r->pool, 0);

	//根据操作类型执行不同数据分析方法,M_GET/M_POST
	switch(r->method_number)
	{
	case M_GET:

		if(!r->args)
		{
			return _param_num;
		}

		_get_post_data = (char*)ap_palloc(r->pool, sizeof(r->args));
		_get_post_data = (char*)ap_pcalloc(r->pool, sizeof(r->args));
		
		//_get_post_data = ap_pstrdup(r->pool, r->args);
		_get_post_data = ap_pstrdup(r->pool, r->args);
		
		//分析GET类型数据
		_param_num = analyseData(r, _get_post_data, _param_list);
		break;
		
	case M_POST:
		
		//获得POST中数据类型
		_content_type = ap_table_get(r->headers_in, "Content-Type");	
		//ap_rprintf(r, "content_type:%s<br>", _content_type);

		//读取POST数据
		_rstat = ap_setup_client_block(r, REQUEST_CHUNKED_DECHUNK);
		
		if(_rstat != OK)
		{
			return _param_num;
		}
		
		//获得post数据大小
		_post_data_len_str = ap_table_get(r->headers_in, "Content-Length");
		_post_data_len = atol(_post_data_len_str);
		
		if(_post_data_len <= 0)
		{
			return _param_num;
		}
		
		//申请内存空间
		//_get_post_data = (char*)ap_palloc(r->pool, (_post_data_len + 1024) * sizeof(char));
		//_get_post_data = (char*)ap_pcalloc(r->pool, (_post_data_len + 1024) * sizeof(char));

		
		//_get_post_data = (char*)ap_palloc(r->pool, _post_data_len + 2048);
		//_get_post_data = (char*)ap_pcalloc(r->pool, _post_data_len + 2048);

		_get_post_data = (char*)calloc(_post_data_len + 1024, sizeof(char));

		if(!ap_should_client_block(r))
		{
			return _param_num;
		}

		///////////////////////////////////////

		char argsbuffer[HUGE_STRING_LEN];
		int rsize, len_read, rpos=0;
		long length = r->remaining;
		*rbuf = ap_pcalloc(r->pool, length + 1);
		ap_hard_timeout("util_read", r);
		while ((len_read =
		ap_get_client_block(r, argsbuffer, sizeof(argsbuffer)))
		> 0) {
		ap_reset_timeout(r);
		if ((rpos + len_read) > length) {
		rsize = length - rpos;
		}
		else {
		rsize = len_read;
		}
		memcpy((char*)*rbuf + rpos, argsbuffer, rsize);
		rpos += rsize;
		}
		ap_kill_timeout(r);
		}

		///////////////////////////////////
		
		/*
		_nbytes = ap_get_client_block(r, _get_post_data, _post_data_len);

		if(_nbytes <= 0)
		{
			return _param_num;
		}
		*/

		ap_rprintf(r, "<h1>%s</h1>", _get_post_data);
		ap_rprintf(r, "<h1>_nbytes:%d</h1>", _nbytes);

		if(strstr(_content_type, "multipart/form-data"))
		{
			//分析multipart类型数据
			//ap_rprintf(r, "<h1>%s</h1>", _content_type);

			int _content_len = atol(ap_table_get(r->headers_in, "Content-Length"));

			//char* m_lpBuffer = (char*)ap_palloc(r->pool, len1 + 1024);

			//int len2 = fread(m_lpBuffer, sizeof(char), len1, stdin);

			//ap_rprintf(r, "<h1>len2:%d</h1>", len2);
			ap_rprintf(r, "<h1>Content-Length:%s</h1>", ap_table_get(r->headers_in, "Content-Length"));
			ap_rprintf(r, "<h1>content-len:%s</h1>", getenv("CONTENT_LENGTH"));

			//_param_num = parse_multi_data(r, _tmp_file_save_path, _content_type, _get_post_data, );

			_param_num = parse_multi_data(r, _content_len, _get_post_data, _param_list);
		}
		else
		{
			//常规post数据处理
			_param_num = analyseData(r, _get_post_data, _param_list);
			
			return _param_num;
		}
	break;
	
	default:
		//默认读取GET数据
		_get_post_data = (char*)ap_palloc(r->pool, sizeof(r->args));
		_get_post_data = (char*)ap_pcalloc(r->pool, sizeof(r->args));
		_get_post_data = ap_pstrdup(r->pool, r->args);
		
		_param_num = analyseData(r, _get_post_data, _param_list);
		break;
	}

	return _param_num;
}


//分析带有附件的POST数据
//html数据中post参数为 enctype=multipart/form-data
//接口类型：private
int parse_multi_data(request_rec* r, int nBytes, char* m_lpBuffer, table** _param_list)
{
	/*******************************************************
	设分隔符为{boundary}，回车(0x0D)和换行符(0x0A)为<CR>
		
	multipart/form-data, boundary={boundary}
	
	1.文件型参数的multipart格式：
	--{boundary}<CR>
	Content-Disposition: form-data; name="参数名称"; filename="文件名称"<CR>
	Content-Type: {Content-Type}<CR>
	<CR>
	文件内容
	<CR>
	--{boundary}<CR>
		
	2.普通参数的multipart格式：
	--{boundary}<CR>
	Content-Disposition: form-data; name="参数名称"<CR>
	<CR>
		参数值
	<CR>
	--{boundary}<CR>
	*******************************************************/
	int _param_num = 0;
	const char* lpEnv;
	char* lpBufEnd;
	int nPos=1;
	
	//lpBufEnd = m_lpBuffer + nBytes;
	lpBufEnd = m_lpBuffer + nBytes;
	*lpBufEnd=0;

	//printf("<h1>lpBufEnd:%s</h1>", lpBufEnd);
	//printf("<h1>lpBufEndLen:%d</h1>", strlen(lpBufEnd));

	if(!(lpEnv= ap_table_get(r->headers_in, "Content-Type")) ||
	!(lpEnv=strstr(lpEnv,"multipart/form-data")) ||
	!(lpEnv=strstr(lpEnv,"boundary=")) )
	{
		//非含有附件操作的post数据
		return;
	}

	ap_rprintf(r, "<h1>lpEnv:%s</h1>", lpEnv);

	lpEnv += sizeof("boundary=") - 3;
	*((unsigned short*)lpEnv) = 0x2d2d;

	//for some proxy,they send multi Boundary split with comma
	int nBoundaryLen=strlen(lpEnv);
	
	char* szFileName = NULL;//附件文件	
	char* lpName = NULL;
	char* lpValue = NULL;
	char* lpTmp = NULL;
	char* lpBoundary = NULL;
	
	lpBoundary=czMemMemI(m_lpBuffer, lpBufEnd - m_lpBuffer, lpEnv, nBoundaryLen);

	//printf("<h1>lpBoundary:%s,  len:%d</h1>", lpBoundary, strlen(lpBoundary));
	
	while( lpBoundary )
	{
		lpName = lpBoundary + nBoundaryLen;
		lpBoundary=czMemMemI(lpName,lpBufEnd-lpName,lpEnv,nBoundaryLen);

		//printf("<h1>lpBoundary:%s,  len:%d</h1>", lpBoundary, strlen(lpBoundary));
		
		if( lpBoundary ){
			lpTmp=lpBoundary;
			if( lpTmp[-1]=='\n'){
				lpTmp--;
				if( lpTmp[-1]=='\r'){
					lpTmp--;
				}
			}
			*lpTmp=0;
		}else{
			lpTmp=lpBufEnd;
		}

		lpName = (char*)strstr(lpName,"Content-Disposition: form-data; name=");

		if( !lpName ){
			continue;
		}

		lpName += sizeof("Content-Disposition: form-data; name=");
		if( !(lpValue=strchr(lpName,'"')) ){
			continue;
		}

		*lpValue++=0;

		char* lpFileName=strstr(lpValue,"filename=");

		if( lpFileName ){
			lpFileName+=sizeof("filename=");
			char* lp0=strchr(lpFileName,'"');
			if( !lp0 ){
				continue;
			}
			
			char* lp1 = strstr(lp0,"Content-Type: ");

			if( !lp1 ){
				lp1=lp0+1;
			}else{
				lp1+=sizeof("Content-Type: ");
			}

			for( ; *lp1!='\n'; lp1++);
			for( lp1++; *lp1!='\n'; lp1++);
			lp1++;

			for( *lp0--=0; lp0>lpFileName; lp0-- ){
				if( *lp0=='\\' || *lp0=='/' ){
					lpFileName=lp0+1;
					break;
				}
			}
			
			if(*lpFileName)
			{
				lpValue = lpBufEnd + nPos;
				nPos += sprintf(lpValue,"%08x%08x%s", time((time_t*)lpValue),lpValue, lpFileName) + 1;

				//strcpy(szFileName,lpValue);
				//待写入的文件路径及名称
				//szFileName = (char*)calloc(strlen(lpValue) + 10, sizeof(char));
				szFileName = (char*)ap_palloc(r->pool, strlen(lpValue) + 10);
				
				sprintf(szFileName, "/tmp/%s", lpValue);

				FILE* fp=fopen(szFileName,"wb");
				
				//printf("<h1>szFileName:%s</h1>", szFileName);
				//printf("<h1>fileLen:%d</h1>", lpTmp - lp1);
				//printf("<h1>contenLen:%d</h1>", strlen(lp1));

				ap_rprintf(r, "<h1>fileLen:%d</h1>", lpTmp - lp1);

				//free(szFileName);
				
				if(fp)
				{
					//fwrite(lp1, 1, lpTmp-lp1, fp);
					fwrite(lp1, 1, lpTmp-lp1 + nBytes, fp);
					fclose(fp);
				}

			}
			else
			{
				lpValue=0;
			}
		}
		else
		{
			for( ; *lpValue!='\n'; lpValue++ );
			for( lpValue++; *lpValue!='\n'; lpValue++ );
			lpValue++;

			ap_unescape_url(lpValue);
			
			//char* decode_str = unescape_url(lpValue);
			//this->cgi_list.addItem(lpName, decode_str, 1);

			//将键、值加入table			
			//ap_table_add(*_param_list, lpName, unescape_url(lpValue));
			_param_num++;
		}
	}
}


////
//接口类型：private
char* czMemMemI(const char* s1, int n1, const char* s2, int n2)
{
	int i = 0;
	if(!n1) return 0;
	for( ; n1>=n2; n1-- ){
		for(i=0; i<n2; i++ ){
			if( s1[i]!=s2[i] ){
				if( s1[i]>='A' && s1[i]<='Z' ){
					if( s1[i]+('a'-'A')!=s2[i] ){
						goto inc_s1;
					}
				} else if( s1[i]>='a' && s1[i]<='z' ) {
					if( s1[i]-('a'-'A')!=s2[i] ){
						goto inc_s1;
					}
				} else {
					goto inc_s1;
				}
			}
		}
		return (char*)s1;
inc_s1:
		s1++;
	}
	return 0;
}


//中文编码转换
//接口类型：private
char x2c(char *what)
{
	register char digit;
	
	digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
	digit *= 16;
	digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
	return(digit);
}

//中文编码转换
//接口类型：private
char* unescape_url(char* url) 
{
    register int i,j;
	
    for(i=0, j=0; url[j]; ++i,++j)
	{		
		if(url[i] == '+')
		{
			url[i] = ' ';
		}
		
        if((url[i] = url[j]) == '%')
		{
            url[i] = x2c(&url[j+1]) ;
            j+= 2 ;
        }
    }
	
    url[i] = '\0' ;
	
	return url;
}
