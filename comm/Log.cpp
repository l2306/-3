#include "Log.h"

void debug_output(const char *fmt , ...)
{
	/*
	time_t ti = time(NULL);
	char times[65535];
	snprintf(times , sizeof(times) , "%s" , ctime(&ti));
	times[strlen(times)-1] = ' ';
	strncat(times , "---   " , 9);
//	*/

	va_list ap;
	char buff[65535];
	va_start(ap , fmt);

	vsnprintf(buff , sizeof(buff) ,  fmt , ap);
	std::cout<<buff;
	//strncat(times , buff , strlen(buff));

	//fwrite(times , sizeof(char) , strlen(times) , pt);
	//fwrite("\n" , 1 , 1 , pt);
	//fflush(pt);
	va_end(ap);
}


Zz_log::Zz_log(string strFile)
{
	pFile = fopen (strFile.c_str(),"a");
	
	pthread_mutex_init(&log_mutex,NULL);
}

Zz_log::~Zz_log()
{
	fflush(pFile);
	fclose(pFile);
}
void Zz_log::getCurTime()
{
	struct tm *time_stamp;
	time_t cur_time;

	cur_time = time(NULL);
	time_stamp = localtime(&cur_time);

	snprintf(str_cur_ts, LEN_TIME_STAMP_BUF, "%02d/%02d/%02d %02d:%02d:%02d",
		time_stamp->tm_mday, time_stamp->tm_mon + 1, time_stamp->tm_year % 100, 
		time_stamp->tm_hour, time_stamp->tm_min, time_stamp->tm_sec);
	return ;
}
void Zz_log::writeLog(string strLog) 
{
	pthread_mutex_lock(&log_mutex);
	getCurTime();
	fputs( str_cur_ts, pFile);
	fputs( strLog.c_str(), pFile);		//
	fwrite("\n" , 1 , 1 , pFile);
	pthread_mutex_unlock(&log_mutex);
}

Zz_logger::Zz_logger()
{
	zz_Log=new Zz_log("cc.log");
}

Zz_logger::~Zz_logger()
{
	delete zz_Log;
}


void Zz_logger::addLog(string strLog)
{
	zz_Log->writeLog(strLog);
}

Zz_logger* Zz_logger::_instance = new Zz_logger();

Zz_logger& operator << (Zz_logger& logger, const char* pch)
{
	logger.addLog(pch);
	return logger;
}

Zz_logger& operator << (Zz_logger& logger, std::string& str)
{
	logger.addLog(str.c_str());
	return logger;
}

