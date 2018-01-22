#ifndef __LOG__
#define __LOG__

#include "def.h"

void debug_output(const char *fmt , ...);

#define dbg() cout<<" dbg "<<__FILE__<<" "<<__FUNCTION__<<" "<< __LINE__<<endl;

#include <string>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

#define LEN_TIME_STAMP_BUF 64

class Zz_log //: public z_Thread
{
public:
	Zz_log(string strFile);
	~Zz_log();
//	void openFile(string strFile);
	void getCurTime();
	void writeLog(string strLog);

//protected:
//	void run();
private:
	FILE* pFile;
	pthread_mutex_t log_mutex ;
	char str_cur_ts[LEN_TIME_STAMP_BUF];
};

class Zz_logger
{

public:
	~Zz_logger();
	void addLog ( string );
	//static Zz_logger& getInstance()
	//{
	//	static Zz_logger *m_pInstance;
	//	if (m_pInstance == NULL) //判断是否第一次调用
	//		m_pInstance = new Zz_logger();
	//	return *m_pInstance;
	//}
	static Zz_logger& instance()
	{
		return *_instance;
	}
private:
	static Zz_logger* _instance;
	Zz_logger();
	Zz_log* zz_Log;
};

Zz_logger& operator << (Zz_logger& logger, const char* pch);
Zz_logger& operator << (Zz_logger& logger, std::string& str);


#endif
