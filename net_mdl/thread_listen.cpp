
#include "thread_listen.h"
#include "ThreadPool.h"

z_threadListen::z_threadListen(z_ThreadPool *pool)
	: _threadPool(pool)
{
	epfd = epoll_create(256);
}

z_threadListen::~z_threadListen()
{
	close(epfd);
}


bool z_threadListen::Bind(unsigned short sPort)
{
	int _sockfd = socket(PF_INET , SOCK_STREAM , 0);
	if(-1 == _sockfd)
	{
		//debug_output("%s\n" , "Create Socket is faild");
		return false;
	}

	// 允许地址重用
	int reuse = 1;
	setsockopt(_sockfd , SOL_SOCKET , SO_REUSEADDR , &reuse , sizeof(reuse));

	bool bRet = setNonBlock(_sockfd);
	if(bRet == false)
	{
		return false;
	}

	struct sockaddr_in servaddr;
	bzero(&servaddr , sizeof(servaddr));

	servaddr.sin_family = PF_INET;
	servaddr.sin_port = htons(sPort);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int nRet = bind(_sockfd , (struct sockaddr*)&servaddr , sizeof(servaddr));
	if(-1 == nRet)
	{
		//debug_output("%s\n" , "bind is faild");
		return false;
	}

	nRet = listen(_sockfd , 128);
	if(-1 == nRet)
	{
		//debug_output("%s\n" , "listen is faild");
		return false;
	}

	struct epoll_event ev;
		bzero(&ev , sizeof(ev));
		ev.data.fd = _sockfd;
		ev.events = EPOLLIN | EPOLLET;

	nRet = epoll_ctl(epfd , EPOLL_CTL_ADD , _sockfd , &ev);
	if(-1 == nRet)
	{
		//debug_output("%s\n" , "epoll_ctl is faild");
		return false;
	}
	continues();
	return true;
}

void z_threadListen::Run()
{
	suspend();

	int nSocket, nfds;
	while(1)
	{
		int nfds = epoll_wait(epfd , events , 256 , 500);
		for(int i=0; i<nfds; i++)
		{
			nSocket = accept(events[i].data.fd , NULL , NULL);
			//debug_output("nSocket = %d\n" , nSocket);
			if(nSocket == -1)
			{
				if(errno == EINTR)
					continue;
				//debug_output("%s\n" , "accept is faild");
			}
			_threadPool->pushSocket(nSocket);   //填加到套接口队列
			_threadPool->continues();   			//通知线程池，运行起来，到套接口队列中取套接口
		}
	}
}

