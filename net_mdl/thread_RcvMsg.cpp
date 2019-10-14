
#include "thread_RcvMsg.h"

z_threadRcvMsg::z_threadRcvMsg(z_ThreadPool *pool) : g_threadPool(pool)
{
	counter = 0;
	epfd = epoll_create(g_numRcvMsgEV);  //自从linux2.6.8之后，size参数是被忽略的

	_data = new z_Data();
}

z_threadRcvMsg::~z_threadRcvMsg()
{
	close(epfd);
	if (_data)
	{
		delete _data;
		_data = NULL;
	}
}

unsigned int z_threadRcvMsg::getCounter()
{
	return counter;
}

void z_threadRcvMsg::addSocket(int nSocket)
{
	setNonBlock(nSocket);
	setSockBuffSize(nSocket , 8*8196 , 8*8196);

	struct epoll_event ev;
	bzero(&ev , sizeof(ev));

	ev.data.fd = nSocket;
	ev.events = EPOLLIN ;		//重要 | EPOLLET
	epoll_ctl(epfd , EPOLL_CTL_ADD , nSocket , &ev);
	counter++;
}

//void z_threadRcvMsg::delSocket(int nSocket)
//{
//	epoll_ctl(epfd , EPOLL_CTL_DEL , nSocket , NULL);
//	counter--;
//}

bool z_threadRcvMsg::pushData(pair_SockData pair_in)
{
	return que_tlvBuffer.push(pair_in);
}

void z_threadRcvMsg::Run()
{
	suspend();    // 暂停线程
	int nfds, sock;
	struct epoll_event ev;
	bool nRet;
	while (1)
	{
		nfds = epoll_wait(epfd, events, g_numRcvMsgEV, 50);	//	maxevents告之内核这个events有多大，这个 maxevents的值不能大于创建epoll_create()时的size
		for (int i = 0; i<nfds; i++)
		{
			if (events[i].events&EPOLLIN)
			{
				if ((sock = events[i].data.fd) < 0)
					continue;
				if ((events[i].events & EPOLLERR) ||  
						(events[i].events & EPOLLHUP) ||  
						(!(events[i].events & EPOLLIN)))  
				{
					fprintf (stderr, "epoll error\n");  
					close (events[i].data.fd);  
					continue;  
				}
				tlv_t* pTLV=NULL;
				if (!(nRet = _data->rcvData(sock, &pTLV)))
				{         						//放到数据池队列中
//					debug_output("--- client quit %d ...\n",sock);
					ev.data.fd = sock;
					//EPOLL_CTL_DEL 参四被忽略可为NULL  
					epoll_ctl(epfd, EPOLL_CTL_DEL, sock, &ev);	
					close(sock);
					events[i].data.fd = -1;
					counter--;
				}
				else													//传到 ThrdPool
				{	
					pair_SockData tmp={ sock , pTLV };
					pushData( tmp ); 
					pair_SockData tmpPair;
					while (que_tlvBuffer.size())
					{
//debug_output("--- client quit %d ...\n",__LINE__);
						que_tlvBuffer.pop(tmpPair);
						g_threadPool->rcvMsg(tmpPair.pTLV, tmpPair.sock);
					}
				}
			}
		}
	}
}
