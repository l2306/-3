#include "Thread.h"
#include "Log.h"

z_Thread::z_Thread()
{
	maskSIGUSR1();

	//	定义SIGUSR1信号阻塞
	sigemptyset(&m_waitSig);
	sigaddset(&m_waitSig, SIGUSR1);
}

z_Thread::~z_Thread()
{
}

void z_Thread::maskSIGUSR1()
{
	sigset_t sig;
	sigemptyset(&sig);
	sigaddset(&sig , SIGUSR1);
	pthread_sigmask(SIG_BLOCK , &sig , NULL);
}


pthread_t z_Thread::getThreadId()
{
	return m_pid;
}


void *z_Thread::threadFun(void *arg)
{
	z_Thread *pThread = (z_Thread*)arg;
	pThread->Run();
}

bool z_Thread::Start()
{
	int nRet = pthread_create(&m_pid , NULL , threadFun , this);
	if(0 == nRet)
	{
		nRet = pthread_detach(m_pid);
		if(nRet == 0)
		{
			return true;
		}
	}else{
		return false;
	}

	return true;
}

void z_Thread::pause()
{
	int sig;
	sigwait(&m_waitSig , &sig);
}

void z_Thread::continues()
{
	pthread_kill(m_pid , SIGUSR1);
}
