
//#define  _CRT_SECURE_NO_WARNINGS 
//不想win下用_s函数 就定义上面变量
#include <stdarg.h>
#include <stdio.h>
#include "_log.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define xxx_fopen(pFILE,fileName,op)	fopen_s(&pFILE, fileName, op);
#define xxx_vsnprintf(logstr, max_siz, format, argp) \
		vsnprintf_s(logstr, max_siz, ((size_t)-1), format, argp);
#define xxx_sprintf					sprintf_s
#define xxx_fprintf					fprintf_s
#endif

#if defined(__unix__) || defined(unix)  
#include <time.h>
#define xxx_fopen(pFILE,fileName,op)	pFILE=fopen(fileName, op);
#define xxx_vsnprintf(logstr, max_siz, format, argp) \
		vsnprintf(logstr, max_siz, format, argp);
#define xxx_sprintf					sprintf
#define xxx_fprintf					fprintf
#endif

const char *Level[] = {
	"FATAL","ALERT","CRIT","ERROR","WARN","NOTICE",
	"INFO","DEBUG","TRACE","NOTSET","UNKNOWN"
};

#define MAXLINSIZE 1024
Log::Log(void)
	:_szFileName(NULL)
{
	_szFileName =(char*) "log/default.log";
	FILE* fp = NULL;
}
Log::~Log(void)
{
	close();
}
bool Log::open(const char* szFileName)
{	
//#if defined(_WIN32) || defined(_WIN64)
//	fopen_s(&fp, szFileName, "at+");
//#endif
//#if defined(__unix__) || defined(unix) 
//	fp =fopen(szFileName, "at+");
//#endif
	xxx_fopen(fp, szFileName, "at+");

	if (NULL == fp)
		return false;
	return true;	
}

bool Log::close()
{
	if(NULL == fp)
		return false;
	fflush(fp);
	fclose(fp);
	return true;
}

const char* getStrTime(char* str)
{
#if defined(_WIN32) || defined(_WIN64)
	SYSTEMTIME 	systime;
	GetLocalTime(&systime);
	sprintf_s(str, "%04d-%02d-%02d %02d:%02d:%02d",
		systime.wYear, systime.wMonth, systime.wDay,
		systime.wHour, systime.wMinute, systime.wSecond);
#endif

#if defined(__unix__) || defined(unix)  
	time_t	tSetTime;
	time(&tSetTime);
	struct	tm* ptm = localtime(&tSetTime);
	sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d",
		ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
		ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
#endif	
	return str;
}

bool Log::LogFileLine( const char *strFile, const int &iLineNum,
	const int &iPriority, const char *format, ...)
{
	char 		logstr[MAXLINSIZE + 1];
	va_list 	argp;
	va_start(argp,format);
	if (NULL == format || 0 == format[0])
		return false;

	xxx_vsnprintf(logstr, MAXLINSIZE, format, argp);
	va_end(argp);
	
	char 		logMsg[2*MAXLINSIZE];
	char		strTime[32];
	xxx_sprintf(logMsg, "[%s] %s | %s | %d | %s ", getStrTime(strTime),
		Level[iPriority], strFile, iLineNum, logstr);

	if (NULL == fp)
		return false;
	xxx_fprintf(fp,"%s\r\n",logMsg);
	return true;
}

bool Log::LogFunction( const char *strFun,	const int &iPriority,
	const char* format,	...)
{
	char 		logstr[MAXLINSIZE + 1];
	va_list 	argp;
	va_start(argp, format);
	if (NULL == format || 0 == format[0])
		return false;

	xxx_vsnprintf(logstr, MAXLINSIZE, format, argp);
	va_end(argp);
	
	char 		logMsg[2 * MAXLINSIZE];
	char		strTime[32];
	xxx_sprintf(logMsg, "[%s] %s | %s | %s", getStrTime(strTime),
		Level[iPriority], strFun, logstr);
		
	if (NULL == fp)
		return false;
	xxx_fprintf(fp,"%s\r\n",logMsg);
	return true;
}

bool Log::LogPrintf( const char* format, ...)
{
	char 		logstr[MAXLINSIZE + 1];
	va_list 	argp;
	va_start(argp, format);
	if (NULL == format || 0 == format[0])
		return false;
 
	xxx_vsnprintf(logstr, MAXLINSIZE, format, argp);

	va_end(argp);
	
	if (NULL == fp)
		return false;
	xxx_fprintf(fp,"%s\r\n",logstr);
	return true;
}
