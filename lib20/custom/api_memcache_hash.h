#ifndef _memcacheHash_h
#define _memcacheHash_h

using namespace std;

class MemcacheHash {
public:
	//���캯��
	MemcacheHash();
	MemcacheHash(char* serv, char* serv_port);

	//��������
	~MemcacheHash();

	//��ʼ��������
	void initMemServ();
	
	//���÷�����
	int setMemServCfg(char* serv, char* serv_port);

	//���key��value���ڴ�cache
	void addHashKey(char* key, char* val, int check_same);

	//����key��value���ڴ�cache
	void setHashKey(char* key, char* val);

	//���keyֵ
	char* getHashKey(char* key);

	//�ͷ��ڴ�������
	void freeMem();

private:
	char memServer[32];
	char memServPort[10];
	struct memcache *mc;//�ڴ�cache�ṹָ��
};

#endif

