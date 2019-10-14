
#include "_singleton.h"

#include <stdio.h>
#include <stdlib.h>

// 替换掉下面的写法 原因每次都要弄一个局部变量  太low
//{																	\
Log log;															\
log.LogFileLine(__FILE__, __LINE__, level, ##__VA_ARGS__);			\
}

// 文件
#define LOG(str)																		\
	Singleton<Log>::instance().open(str);		

// 写log		带文件名、行号、时间和信息
#define LOG_FILE(level,...)																\
	Singleton<Log>::instance().LogFileLine(__FILE__, __LINE__, level, ##__VA_ARGS__);	\

// 写log		带模块名，时间和信息的 log
#define LOG_FUN(level,...)																\
	Singleton<Log>::instance().LogFunction(__FUNCTION__, level, ##__VA_ARGS__);			\

// 写log		与printf一样，具体格式有用户定义；
#define LOG_PRINTF(fmt,...)																		\
	Singleton<Log>::instance().LogPrintf(fmt,##__VA_ARGS__);				


typedef enum {
	/** fatal	*/		LOG_LEVEL_FATAL = 000,
	/** alert	*/		LOG_LEVEL_ALERT,
	/** crit	*/		LOG_LEVEL_CRIT,
	/** error	*/		LOG_LEVEL_ERROR,
	/** warn	*/		LOG_LEVEL_WARN,
	/** notice	*/		LOG_LEVEL_NOTICE,
	/** info	*/		LOG_LEVEL_INFO,
	/** debug	*/		LOG_LEVEL_DEBUG,
	/** trace	*/		LOG_LEVEL_TRACE,
	/** notset	*/		LOG_LEVEL_NOTSET,
	/** unknown	*/		LOG_LEVEL_UNKNOWN
}enum_LOG_LEVEL;

#define LOG_INFO	LOG_LEVEL_INFO
#define LOG_WARN	LOG_LEVEL_WARN
#define LOG_ERROR	LOG_LEVEL_ERROR
#define LOG_DEBUG	LOG_LEVEL_DEBUG

class Log
{
public:
	Log(void);
	~Log(void);
	char*	_szFileName;
	FILE*	fp;
	bool open(const char* szFileName);
	bool close();
	
	// 带时间文件路径和行号的写日志格式；
	bool LogFileLine(
		const char*	strFile,
		const int &	iLineNum,
		const int &	iPriority,
		const char* format,
		...
	);
	// 带时间和功能模块名的日志；
	bool LogFunction(
		const char*	strFun,
		const int&	iPriority,
		const char*	format,
		...
	);
	// 日志内容由客户自己定义 ;
	bool LogPrintf(
		const char* format,
		...
	);
};


