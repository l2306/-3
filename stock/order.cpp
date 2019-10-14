
#include "order.h"

string str_enumBuySell[4]={"--", "买", "卖", "撤"};

string getStrCurTime() 
{  
    timespec time;
    clock_gettime(CLOCK_REALTIME, &time);  //获取相对于1970到现在的秒数
    tm timeNow;
    localtime_r(&time.tv_sec, &timeNow);
    char current[64];
    sprintf(current, "%02d.%02d.%02d_%02d:%02d:%02d.", 
		timeNow.tm_year -100, timeNow.tm_mon+1, timeNow.tm_mday, 
		timeNow.tm_hour, timeNow.tm_min, timeNow.tm_sec);
	return current;

//	struct tm *t;
//	time_t tt;
//	time(&tt);
//	t = localtime(&tt);
//	printf("%4d年%02d月%02d日 %02d:%02d:%02d\n",
//		t->tm_year +1900, t->tm_mon +1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}

#include <time.h> 
#include <sys/time.h> 
string sysTime_usec() 
{ 
	struct timeval	tv; 
	struct timezone	tz; 
	struct tm		*p; 
 
	gettimeofday(&tv, &tz); 
//    printf("tv_sec:%ld\n",  tv.tv_sec); 
//    printf("tv_usec:%ld\n",  tv.tv_usec); 
//    printf("tz_minuteswest:%d\n",  tz.tz_minuteswest); 
//    printf("tz_dsttime:%d\n",  tz.tz_dsttime); 
       
	p = localtime(&tv.tv_sec); 
	char current[64];
	sprintf(current,"%02d.%02d.%02d_%02d:%02d:%02d.%06ld\n",
		p->tm_year-100, 1+p->tm_mon, p->tm_mday,
		p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec); 
	return current;
} 


ostream& operator<<(ostream &os, const Order* p){
	os << str_enumBuySell[p->_eBuySell]<<" "<< p->_account_number<<" "//<<p->_time_stamp
		<<" 价:"<< p->_price <<" 余:"<< p->_quantity_needdeal;
	return os;
}

int deal_do(Order* pToDo,Order* pWait,int done){
	pToDo ->_quantity_needdeal -= done;
	pWait ->_quantity_needdeal -= done;
	if( eBUY==pToDo->_eBuySell )
		;
	else if( eSELL==pToDo->_eBuySell )
		;
	cout <<"    成交量 :"<< done <<"\n     / "<< pToDo <<" \n     \\ "<< pWait <<endl;
}


