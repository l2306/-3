//
//线程的封装 , 子类继承并实现自己的功能


#ifndef __THREAD__
#define __THREAD__

#include "def.h"

class z_Thread
{
	public:
		//	构造函数
		z_Thread();

		//	析构函数
		~z_Thread();

		//	获得线程id
		//		线程id
		pthread_t getThreadId();

		//	启动线程
		bool Start();

		//	线程停止
		void pause();

		//	线程继续运行
		void continues();

	private:
		// 屏蔽SIGUSR1信号 , POSIX标准建议在调用sigwait()等待信号以前，进程中所有线程都应屏蔽该信号，以保证仅有sigwait()的调用者获得该信号 ,
		//	 如果不屏蔽该信号，在sigwait()之前调用pthread_kill()就会出现User defined signal 1.
		void maskSIGUSR1();

		//	线程pid
		pthread_t m_pid;

		//	信号
		sigset_t m_waitSig;

		//	线程运行主函数
		//		arg	看使用
		static void *threadFun(void *arg);

	protected:
		virtual void Run() = 0;

};

#endif
