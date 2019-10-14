/*
	在程序异常退出时，保存堆栈信息到文件
*/


#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <execinfo.h>
#include <signal.h>

#ifdef  __cplusplus  
extern "C" {  
#endif  

//gcc -g -rdynamic -Wall -Werror -std=gnu99 app_exit_info.c 
//注意: 
//	不加 -g， addr2line程序无法打印行号；
//	不加 -rdynamic， backtrace无法打印完整信息，可能会没有和函数名称相关的信息；
//若本奔溃在  c++代码里面  输出文件中函数名可能有一些前缀
//	解决反感：  $ cat core.2018-6-28_0-58-22 |c++filt
/*
使用addr2line解析出文件名和行号 		（注意是文件名而不是函数名） 
	addr2line -e <执行文件> <代码地址>	(后加 -f 可显示代码所在函数)
*/

//  对外接口 安装程序退出函数
void inst_exit_func();

// 获取程序堆栈打印到文件中
void app_crash_backtrace_2_file(int sig);

void server_on_exit(void){
		//do something when process exits
}

void signal_crash_handler(int sig){
	app_crash_backtrace_2_file(sig);
	exit(-1);
}

void signal_exit_handler(int sig){
	exit(0);
}

void inst_exit_func()
{
	//注册退出函数
	atexit(server_on_exit);

	signal(SIGTERM, signal_exit_handler);	// kill进程
	signal(SIGINT, signal_exit_handler);	// ctrl-c结束前台进程
	
	signal(SIGPIPE, SIG_IGN);		// ignore SIGPIPE
	
	//避免系统默认的回调，系统默认的回调可能就是直接crash掉程序
	signal(SIGBUS, signal_crash_handler);	// 总线错误
	signal(SIGSEGV, signal_crash_handler);	// SIGSEGV，非法内存访问
	signal(SIGFPE, signal_crash_handler);	// SIGFPE，数学相关的异常，如被0除，浮点溢出，等等
	signal(SIGABRT, signal_crash_handler);	// SIGABRT，由调用abort函数产生，进程非正常退出
}

#define TEST_EXIT_INFO
#ifdef TEST_EXIT_INFO
int test_app_eixt_info()
{
	inst_exit_func();
//	while(1)
	{
		sleep(10);
	}

	int* p =0;
	*p = 11;

	return 0;
}
#endif

void app_crash_backtrace_2_file(int sig)
{
	//打开文件
	time_t	tSetTime;
	time(&tSetTime);
	struct	tm* ptm = localtime(&tSetTime);
	char	fname[256] = {0};
	sprintf( fname, "core.%d-%02d-%02d_%02d-%02d-%02d",
            ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec );
	FILE* 	f = fopen( fname, "a" );
	if ( f == NULL ){
		return;
	}
	int 	fd = fileno( f );

	//锁定文件
	struct flock fl;
	fl.l_type = F_WRLCK;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;
	fl.l_pid = getpid();
	fcntl(fd, F_SETLKW, &fl);

	//输出程序的绝对路径
	char buffer[4096];
	memset( buffer, 0, sizeof(buffer) );
	int count = readlink( "/proc/self/exe", buffer, sizeof(buffer) );
	if( count > 0 ){
		buffer[count] = '\n';
		buffer[count + 1] = 0;
		fwrite( buffer, 1, count+1, f );
	}

	//输出信息的时间
	memset( buffer, 0, sizeof(buffer) );
	sprintf( buffer, "Dump Time: %d-%02d-%02d %02d:%02d:%02d\n",
            ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec );
	fwrite( buffer, 1, strlen(buffer), f );

	//线程和信号
	sprintf( buffer, "Curr thread: %x, Catch signal:%2d  %s\n",
		(long)pthread_self(), sig , strsignal(sig) );
	fwrite( buffer, 1, strlen(buffer), f );

	//堆栈
	void*	DumpArray[256];
	int		nSize = backtrace(DumpArray, 256);
	sprintf( buffer, "backtrace rank = %d\n", nSize );
	fwrite( buffer, 1, strlen(buffer), f );
	if ( nSize > 0 ){
		char** symbols = backtrace_symbols( DumpArray, nSize );
		if ( symbols != NULL ){
			//int i=0;
			for ( int i=0; i<nSize; i++ ){
				fwrite( "\t", 1, 1, f );
				fwrite( symbols[i], 1, strlen(symbols[i]), f );
 				fwrite( "\n", 1, 1, f );
			}
			free( symbols );
		}
	}

	//文件解锁后关闭
	fl.l_type = F_UNLCK;
	fcntl( fd, F_SETLK, &fl );
	fclose( f );
}

#ifdef  __cplusplus  
}
#endif  
