
#include "zz_server.h"
#include "ThreadPool.h"

z_Server *z_Server::instance = NULL;

static void hdlCtrlC(int signum)
{
	printf("ctrl-c signal .....\n");
	z_Server::delself();
}

z_Server::z_Server()
{
	instance = this;
}

z_Server::~z_Server()
{
}


bool z_Server::init()
{
	printf("%s\n" , __PRETTY_FUNCTION__);
	
	struct sigaction act;
	act.sa_handler = hdlCtrlC;   
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT , &act , NULL);		//定义ctrl-c信号
}

z_Worker::z_Worker(unsigned int num)
{
	_threadPool = new z_ThreadPool(num , this);
}

z_Worker::~z_Worker()
{
	if(_threadPool)
	{
		delete _threadPool;
		_threadPool = NULL;
	}
}

bool z_Worker::bind(unsigned int nPort)
{
	return _threadPool->bind(nPort);
}

int z_Worker::sndMsg(int nSocket, tlv_t *pTLV)
{
	if(!pTLV)
		return -1;
	return _threadPool->sndMsg(nSocket, pTLV);
}
