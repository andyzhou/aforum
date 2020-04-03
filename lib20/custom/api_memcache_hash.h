#ifndef _memcacheHash_h
#define _memcacheHash_h

using namespace std;

class MemcacheHash {
public:
	//构造函数
	MemcacheHash();
	MemcacheHash(char* serv, char* serv_port);

	//析构函数
	~MemcacheHash();

	//初始化服务器
	void initMemServ();
	
	//设置服务器
	int setMemServCfg(char* serv, char* serv_port);

	//添加key、value到内存cache
	void addHashKey(char* key, char* val, int check_same);

	//更新key、value到内存cache
	void setHashKey(char* key, char* val);

	//获得key值
	char* getHashKey(char* key);

	//释放内存操作句柄
	void freeMem();

private:
	char memServer[32];
	char memServPort[10];
	struct memcache *mc;//内存cache结构指针
};

#endif

