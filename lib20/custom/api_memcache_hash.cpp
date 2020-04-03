#include <err.h>
#include <sysexits.h>
#include <stdio.h>
#include <stdlib.h>
//#include <iostream.h>
#include <string.h>
#include <unistd.h>

#include "memcache.h"
#include "MemcacheHash.h"

using namespace std;

//���캯��
MemcacheHash::MemcacheHash()
{
	memset(this->memServer, 0, sizeof(this->memServer));
	memset(this->memServPort, 0, sizeof(this->memServPort));

	//ʵ�����ڴ�cache�ṹָ��
	this->mc = mc_new();
}


//���ع��캯��
MemcacheHash::MemcacheHash(char* serv, char* serv_port)
{
	int ret = 0;

	/* 0:	success
	 * -1:	Unable to allocate a new server instance
	 * -2:	Unable to strdup hostname
	 * -3:	Unable to strdup port
	 * -4:	Unable to Unable to resolve the host, server deactivated, but added to list
	 * -5:	Unable to realloc(3) the server list, server list unchanged */

	memset(this->memServer, 0, sizeof(this->memServer));
	memset(this->memServPort, 0, sizeof(this->memServPort));

	sprintf(this->memServer, "%s", serv);
	sprintf(this->memServPort, "%s", serv_port);

	//ʵ�����ڴ�cache�ṹָ��
	this->mc = mc_new();

	//add server
	ret = mc_server_add(this->mc, this->memServer, this->memServPort);

	if(ret != 0)
	{
		switch(ret)
		{
		case -1:
			cout << "Unable to allocate a new server instance" << endl;
			break;
			
		case -2:
			cout << "Unable to strdup hostname:" << this->memServer << endl;
			break;
			
		case -3:
			cout << "Unable to strdup port:" << this->memServPort << endl;
			break;
			
		case -4:
			cout << "Unable to Unable to resolve the host, server deactivated, but added to list" << endl;
			break;
			
		case -5:
			cout << "Unable to realloc(3) the server list" << endl;
			break;
		}
		exit(1);
	}
}


//��������
MemcacheHash::~MemcacheHash()
{
}

//�ͷ��ڴ�������
void MemcacheHash::freeMem()
{
	mc_free(this->mc);
}

//��ʼ��������
void MemcacheHash::initMemServ()
{
	//ʵ�����ڴ�cache�ṹָ��
	this->mc = mc_new();
}


//���÷�����
//1: ok 0: failed
int MemcacheHash::setMemServCfg(char* serv, char* serv_port)
{
	int ret;

	if(!serv || !serv_port)
	{
		return 0;
	}

	memset(this->memServer, 0, sizeof(this->memServer));
	memset(this->memServPort, 0, sizeof(this->memServPort));

	sprintf(this->memServer, "%s", serv);
	sprintf(this->memServPort, "%s", serv_port);

	//add server
	ret = mc_server_add(this->mc, this->memServer, this->memServPort);

	//printf("%s, %s\n", this->memServer, this->memServPort);
	//cout << "ret_ret:" << ret << endl;

	return (ret == 0) ? 1 : 0;
}


//���key��value���ڴ�cache
void MemcacheHash::addHashKey(char* key, char* val, int check_same)
{
	mc_add(this->mc, key, strlen(key), val, strlen(val), 0, 0);
}


//����key��value���ڴ�cache
void MemcacheHash::setHashKey(char* key, char* val)
{
	mc_set(this->mc, key, strlen(key), val, strlen(val), 0, 0);
}


//���keyֵ
char* MemcacheHash::getHashKey(char* key)
{
	return (char*)mc_aget(this->mc, key, strlen(key));
}

/*
//demo
int main()
{
	MemcacheHash imh;

	imh.setMemServCfg("127.0.0.1" , "11211");

	//imh.setHashKey("60d2f4fe0275d790764f40abc6734499", "diudiu");

	printf("name:%s\n", imh.getHashKey("60d2f4fe0275d790764f40abc6734499"));

	return 0;
}
*/





