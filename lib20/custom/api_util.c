/*
* Copyright (c) 2006
* All rights reserved.
* 
* file name��api_util.c
* file desc��common func list
* author:AndyZhou<diudiu8848@163.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>


#include "api_util.h"

//����Ҫ���ַ����ձ�
int acsii_table[] = {58,59,60,61,62,63,64,73,79,91,92,93,94,95,96,108,111};

int low_ascii_bound =50; // "2" ��ʼASCIIֵ
int upper_ascii_bound =122; // "z" ����ASCIIֵ


//�ж�һ��˫�ֽ��ַ��Ƿ���GBK���뺺��
//1:true 0:false
int is_chinese(const unsigned char ch1, const unsigned char ch2)
{
	if((ch1 >= 0x81 && ch1 <= 0xFE) && ((ch2 >= 0x40 && ch2 <= 0x7E) || (ch2 >= 0xA1 && ch2 <= 0xFE)))
	{
		return 1;
	}

	return 0;
}

//�ִ���ȡ��֧������, �ɹ��󷵻ط���Ҫ����ִ���
int getSubStr(request_rec* r, char** target, const char* source, int start, int length)
{
	int strLen = 0;//�ַ�����
	int beginLen = 0;//ʵ�ʿ�ʼλ��
	int tmpStart = 0;
	int suitLen = 0;//ȷ���ַ�����

	int m = 0;
	int n = 0;
	int i = 0;
	int y = 0;
	int z = 0;

	static char tmp_str[sizeof(char)] = {0};

	if(!source || start < 0 || length <= 0)
	{
		return suitLen;
	}

	strLen = strlen(source);

	if(length >= strLen && start == 0)
	{
		*target = (char*)ap_palloc(r->pool, (strLen + 1) * sizeof(char));
		*target = (char*)ap_pcalloc(r->pool, (strLen + 1) * sizeof(char));
		*target = ap_pstrdup(r->pool, source);

		return strLen;
	}

	if(start > 0)
	{
		length += start;
	}

	for(i = 0; i < strLen; i++)
	{
		if(m > (length - 1))
		{
			break;
		}

		if(is_chinese(source[i], source[i+1]))
		{
			//cout << "chinese charset" << endl;
			i += 1;
			suitLen += 2;

			if(start >= 0 && tmpStart < start)
			{
				beginLen += 2;
			}
		}
		else
		{
			//cout << "normal charset" << endl;
			suitLen += 1;

			if(start > 0 && tmpStart < start)
			{
				beginLen += 1;
			}
		}

		if(start >= 0 && tmpStart < start)
		{
			tmpStart++;
		}
		
		m++;
	}

	*target = (char*)ap_palloc(r->pool, (suitLen + 1) * sizeof(char));
	*target = (char*)ap_pcalloc(r->pool, (suitLen + 1) * sizeof(char));
	
	for(y = beginLen, z = 0; y < suitLen; y++, z++)
	{
		(*target)[z] = source[y];
	}

	return z;
}


//�ַ�(��)�滻, tokstr:���滻���ִ� word:�滻���ִ�
void replaceStr(request_rec* r, const char* source_str, char** target_str, 
					 char* tokstr, char* word)
{
	int maxstrlen = 0;
	int tokstrlen = 0;
	int new_size = 0;

	char* srcstr = 0;
	char* newstr = 0;
	char* newp = 0;
	char* srcp = 0;
	char* tokenp = 0;
	char* retp = 0;

	srcstr = (char*)source_str;

	//ap_rprintf(r, "<h1>srcstr:%s, tokstr:%s, word:%s</h1>", srcstr, tokstr, word);
	
	/* ��ʼ���ַ���ָ�� */
	newstr = newp = srcp = tokenp = retp = 0;

	if(strlen(word) > strlen(tokstr))
	{
      maxstrlen
      = ((strlen(srcstr) / strlen(tokstr)) * strlen(word))
      + (strlen(srcstr)
      % strlen(tokstr));
	}
    else
	{
		maxstrlen = strlen(srcstr);
	}

    //target_str = (char *)malloc((maxstrlen + 1) * sizeof(char));
	//newstr = new char((maxstrlen + 1) * sizeof(char));
	//memset(newstr, 0, (maxstrlen + 1) * sizeof(char));
	
	*target_str = (char*)apr_palloc(r->pool, (maxstrlen + 1) * sizeof(char));
	*target_str = (char*)apr_pcalloc(r->pool, (maxstrlen + 1) * sizeof(char));

	//spool = ap_make_sub_pool(r->pool);
	//*target_str = (char*)apr_palloc(r->pool, maxstrlen + 1);
	//ap_rprintf(r, "<h1>2, maxstrlen:%d</h1>", maxstrlen);

	//newstr = (char*)apr_palloc(r->pool, maxstrlen + 1);
	//newstr = (char*)apr_pcalloc(r->pool, maxstrlen + 1);

    tokstrlen = strlen(tokstr);

    for(srcp = srcstr, newp = *target_str; *srcp; srcp++) 
	   {	

      if(!strncmp(srcp, tokstr, tokstrlen)) 
		{
        	char *wordp;
        	for(wordp = word; *wordp; wordp++)
			{
				 *newp++ = *wordp;
			}
      	srcp += tokstrlen - 1;
     		 }
      else
		{
			*newp++ = *srcp;
		}
	   }

    *newp = '\0';


	
	//retp = newstr;
	//*target_str = newstr;

	//ap_rprintf(r, "<h1>target_str:%s</h1>", *target_str);

	//�����滻����ִ�����ǰ����
	//*target_str = ap_pstrdup(p, newstr);

	return;
}


//������ֵ����������
//65-90 A-Z
//97-122 a-z
int mtRand(int begin_num, int end_num)
{
	char num[10];
	int tmpNum = 0;
	int i = 0;

	if(begin_num <=0 || end_num <= 0)
	{
		return tmpNum;
	}

	if(begin_num >= end_num)
	{
		return tmpNum;
	}

	//���������
	//srand( (unsigned)time(NULL));

	for(i=0; ;i++)
	{
		memset(num, 0, 10);
		sprintf(num, "%d", rand());

		tmpNum = atol(num)%end_num;//ȡ����

		if(tmpNum <= end_num && tmpNum >= begin_num)
		{
			return tmpNum;
		}
	}
}
	
//�ж�ĳ��ֵ�Ƿ���ĳ������Ԫ����
int isInArray(int num, int tmpArray[], int enumNum)
{
	int i = 0;

	for(i = 0; i < enumNum; i++)
	{
		if(num == tmpArray[i])
		{
			return 1;
		}
	}
	return 0;
}

//��������ִ�
char* genRandString(int rand_str_length)
{
	int i;
	int randNum;//�����
	char tmpStr[2];
	static char targetStr[32] = {0};

	i=0;

	//���ʱ������
	srand((unsigned)time(NULL));
	
	while(i < rand_str_length)
	{
		//����һ�������
		randNum = mtRand(low_ascii_bound, upper_ascii_bound);

		if(!isInArray(randNum, acsii_table, 17))
		{
			if(i==0)
			{
				sprintf(targetStr, "%c", randNum);
			}
			else
			{
				memset(tmpStr,0,2);
				sprintf(tmpStr,"%c",randNum);
				strcat(targetStr,tmpStr);
			}

			i++;
		}
	}

	return targetStr;
}

//����hash������hash·��
char* computeHashPath(char* hash_str, int hash_max_num)
{
	static char tmp_path[5] = {0};
	static char suit_hash_path[20] = {0};
	int i = 0;
	int rand_num = 0;

	//ȡǰ�����ַ���ת�����������Ӧhashֵ
	for(i = 0; i < 3; i++)
	{
		//�������ַ�ת��Ϊasciiֵ
		rand_num = toascii(hash_str[i]) & hash_max_num;

		if(i == 0)
		{
			sprintf(suit_hash_path, "%d", rand_num);
		}
		else
		{
			memset(tmp_path, 0, 5);
			sprintf(tmp_path, "/%d", rand_num);
			strcat(suit_hash_path, tmp_path);
		}
	}

	return suit_hash_path;
}


//��ָ���ָ���ֽ��ִ���array_header����������
//����ƥ��Ľ����
int splitStr(request_rec* r,  char* source_str, 
			 const char tokchar, apr_array_header_t** arrList)
{
	int rec_num = 0;
	char* _val = 0;
	char* tmp_val = 0;
	char** arr_val = 0;
	
	if(!source_str || strlen(source_str) < 1)
	{
		return rec_num;
	}

	if(!tokchar || tokchar == '\0')
	{
		return rec_num;
	}

	//��ʼ���洢��
	*arrList = ap_make_array(r->pool, 1, sizeof(char*));

	//���Դ�ִ����Ƿ��зֽ��,��û��ֱ�Ӵ洢�����ִ���array_header
	if(!strchr(source_str, tokchar))
	{
		rec_num = 1;

		arr_val = (char**)ap_push_array(*arrList);
		*arr_val = ap_pstrdup(r->pool, source_str);

		return rec_num;
	}

	while (*source_str != '\0')
	{
		_val = ap_getword_nc(r->pool, &source_str, tokchar);

		if(_val && strcmp(_val, ""))
		{
			arr_val = (char**)ap_push_array(*arrList);
			*arr_val = ap_pstrdup(r->pool, _val);

			rec_num++;
		}
	}

	return rec_num;
}


//������鵥��Ԫ��ֵ
char* getArrEnum(apr_array_header_t* arrList, int no)
{
	char **list = 0;

	list = (char**)arrList->elts;

	if(!list)
	{
		return NULL;
	}

	return list[no] ? list[no] : NULL;
}


//URL����
const char url_digit[20]="0123456789ABCDEF";

char* urlEscape(char* buffer, int blen, const char* url)
{
	int i = 0;

	if(!(url && buffer))
	{
		return NULL;
	}

	while((*url) && (i < blen-1))
	{
		if( ((*url >= 'a')&&(*url <= 'z'))
			|| ((*url >= 'A')&&(*url <= 'Z'))
			|| ((*url >= '0')&&(*url <= '9'))
			)
		{
			buffer[i] = *url;
			i++;
		}
		else
		{
			if(i < blen - 3 )
			{
				buffer[i] = '%';
				i++;
				buffer[i] = url_digit[(*url >> 4) & 0x0f];
				i++;
				buffer[i] = url_digit[*url & 0x0f];
				i++;
			}
		}
		url ++;
	}
	buffer[ i ] = '\0';
	return buffer;
}


//�ַ��Ƴ�
//1:ok 0:fail
int charRemove(char* buffer, int blen, 
			   const char* src_str, char sig_char)
{	
	int i = 0;

	if(!(src_str && buffer))
	{
		return 0;
	}

	while((*src_str) && (i < blen-1))
	{
		if(*src_str != sig_char)
		{		
			buffer[i] = *src_str;

			i++;
		}
		src_str++;
	}
	
	buffer[i] = '\0';

	return 1;
}


//ȥ���ո�
char* stringTrim(char str[])
{
  int firstchar=0;
  int endpos=0;
  int i;
  int firstpos=0;

  for(i=0;str[i]!='\0';i++){
    if(str[i]==' ' || str[i] == '\r' || str [i] == '\n' || str [i]=='\t'){
      if(firstchar==0) firstpos++;
    }
    else{
      endpos=i;
      firstchar=1;
    }
  }

  for(i=firstpos;i<=endpos;i++)
    str[i-firstpos]=str[i];
  str[i-firstpos]='\0';

  return str;
}


//�ж��ִ��Ƿ�Ϊ����
//1:ok 0:fail
int isNumeric(char str[])
{
	int i = 0;

	for(i = 0; str[i] != '\0'; i++)
	{
		if(str[i] > '9' || str[i] < '0')
		{
			return 0;
		}
	}

	return 1;
}


//��ȡ�����ļ�����������Ϣģ��
char* getIniValue(char* iniFileContent, char* sectName, char* parameter)
{
	char * pItem;
	char * pValue;//����Ҫ���ini����ֵ
	char * pLine;
	int bSectOk = 0;//�Ƿ��ҵ��ڵ�

	char *nvPair=NULL;
	char *suitVal=NULL;

	//��ini���ݸ��Ƶ���ʱ����
	int len;
	len=strlen(iniFileContent);
	char tmpStr[len+2];
	memset(tmpStr,0,len+2);

	strcpy(tmpStr,iniFileContent);

	//printf("<b><font color=blue>%s,%s</font></b><br>",sectName,parameter);

	//��ini��\r���ŷ���ƥ��
	nvPair=strtok(tmpStr,"\n");

	//�����¸�һ����ֵ�ִ�
	while(nvPair!=NULL)
	{
		//�����һ������Ҫ������
        nvPair = strtok(NULL,"\n");

		//printf("<h1>%s</h1>",nvPair);

		/*ȥ�����׵Ŀո���Ʊ��*/		
		pLine = (char*)stringTrim(nvPair);
		
		/*�Ƿ�Ϊ����*/
		if (strlen(pLine)<3)
    	continue;

		/*�Ƿ�Ϊע����*/
		if ( (*pLine)=='#' )
		continue;

		/*��С������*/
		if ( ((*pLine)=='[' ) && (*(pLine + strlen(pLine)-1)==']') )
		{
			pLine ++;
			*(pLine + strlen(pLine)-1) = '\0';
			pLine = (char*)stringTrim(pLine);
			if (strcasecmp(pLine, sectName)==0)
			{
				bSectOk = 1;
			}
			else
			{
				bSectOk = 0;
			}

			//printf("<h1>%d</h1>",bSectOk);
		}	

		/*�ڸ�����С����*/
		if (bSectOk)
		{	
			//printf("<b><font color=red>%s</font></b><br>",pLine);

			/*�� '=' ��*/
			pItem = pLine;
			if ((pValue = strchr(pLine, '=')) == NULL)
			continue;
			/*�����������ֵ*/
			pLine = pValue;
			pValue ++;
			*pLine = '\0';
			pItem = (char*)stringTrim(pItem);
			/*�Ǹ���������*/
			if (strcasecmp(pItem, parameter)==0)
			{
				pValue = stringTrim(pValue);
				//printf("<h1><font color=red>%s,%s</font></h1>",pItem,pValue);
				break;
			}
		}
		
	}

	//�ҵ�����Ҫ��Ľڵ㡢��
	if(pLine && pValue)
	{
		return pValue;
	}

	return NULL;
}

