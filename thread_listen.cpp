
#include "thread_listen.h"

z_threadListen::z_threadListen(z_ThreadPool *pool)
	: m_threadPool(pool)
{
	epfd = epoll_create(256);
}

z_threadListen::~z_threadListen()
{
	close(m_sockfd);
	close(epfd);
}

bool z_threadListen::Bind(unsigned short usPort)
{
	m_sockfd = socket(PF_INET , SOCK_STREAM , 0);
	if(-1 == m_sockfd)
	{
		debug_output("%s\n" , "Create Socket is faild");
		return false;
	}

	// 允许地址重用
	int reuse = 1;
	setsockopt(m_sockfd , SOL_SOCKET , SO_REUSEADDR , &reuse , sizeof(reuse));

	bool bRet = setNonBlock(m_sockfd);
	if(bRet == false)
	{
		return false;
	}

	struct sockaddr_in servaddr;
	bzero(&servaddr , sizeof(servaddr));

	servaddr.sin_family = PF_INET;
	servaddr.sin_port = htons(usPort);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int nRet = bind(m_sockfd , (struct sockaddr*)&servaddr , sizeof(servaddr));
	if(-1 == nRet)
	{
		debug_output("%s\n" , "bind is faild");
		return false;
	}

	nRet = listen(m_sockfd , 128);
	if(-1 == nRet)
	{
		debug_output("%s\n" , "listen is faild");
		return false;
	}

	struct epoll_event ev;
	bzero(&ev , sizeof(ev));
	ev.data.fd = m_sockfd;
	ev.events = EPOLLIN | EPOLLET;

	nRet = epoll_ctl(epfd , EPOLL_CTL_ADD , m_sockfd , &ev);
	if(-1 == nRet)
	{
		debug_output("%s\n" , "epoll_ctl is faild");
		return false;
	}

	continues();
	return true;
}

bool z_threadListen::Listen(int &nSocket)
{
	static int counter = 0;
	int nfds , i;
	while(1)
	{
		nfds = epoll_wait(epfd , events , 256 , 500);
		for(i=0; i<nfds; i++)
		{
			if(events[i].data.fd == m_sockfd)
			{
				nSocket = accept(m_sockfd , NULL , NULL);
//				debug_output("nSocket = %d\n" , nSocket);
				if(nSocket == -1)
				{
					if(errno == EINTR)
					{
						continue;
					}
					debug_output("%s\n" , "accept is faild");
					return false;
				}
//				debug_output("counter = %d\n" , ++counter);
				return true;
			}
		}
	}
}

void z_threadListen::Run()
{
	pause();
	debug_output("Listen thread is starting ....\n");
	
	int nSocket; 
	while(1)
	{
		if(Listen(nSocket))
		{
//			debum_output("new client is connecting ....\n");
//setSockBufSize(nSocket,1024*64,1024*64);
			m_threadPool->pushSocket(nSocket);   //填加到套接口队列
			m_threadPool->continues();   //通知线程池，运行起来，到套接口队列中取套接口
		}
	}
}


