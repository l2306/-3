
#include "thread_SndMsg.h"

z_threadSndMsg::z_threadSndMsg()
{
	bzero(&ev , sizeof(struct epoll_event));
	epfd = epoll_create(256);
	_data = new z_Data();
}

z_threadSndMsg::~z_threadSndMsg()
{
	close(epfd);
	if(_data)
	{
		delete _data;
		_data = NULL;
	}
}

void z_threadSndMsg::addEpoll(int nSocket)
{
	ev.data.fd = nSocket;
	ev.events = EPOLLOUT | EPOLLERR;
	epoll_ctl(epfd , EPOLL_CTL_ADD , nSocket , &ev);
}

void z_threadSndMsg::modEpoll(int nSocket)
{
	ev.data.fd = nSocket;
	ev.events = EPOLLOUT | EPOLLERR;
	epoll_ctl(epfd , EPOLL_CTL_MOD , nSocket , &ev);
}

void z_threadSndMsg::delEpoll(int nSocket)
{
	ev.data.fd = nSocket;
	epoll_ctl(epfd , EPOLL_CTL_DEL , nSocket , &ev);
}

int z_threadSndMsg::sndMsg(int nSocket, tlv_t *pTLV)
{
	mapSendData.insert(pair_mapSendData(nSocket, pTLV));	//把套接口和数据插入map中
	return 0;
}

//
void z_threadSndMsg::Run()
{
	multimap<int, tlv_t*>::iterator map_it;

	static int cnt = 0;
	static int cnt2 = 0;

	int nfds, i, sock;
	string buffer;
	while (1)
	{
		nfds = epoll_wait(epfd, event, 100, 1000*1000*10);		//？？要用epoll
		for (i = 0; i<nfds; i++)
		{
			if (event[i].events&EPOLLOUT)
			{
				sock = event[i].data.fd;
				while (1)
				{
					map_it = mapSendData.find(sock);
					if (map_it == mapSendData.end())
						break;

					_data->sndData(sock, map_it->second);
					//if(map_it->second->tag)
					//		cout<<" tag2 "<<++cnt2<<" ";
					//	debug_output("--- client quit %d ...\n",sock);
					//	free(map_it->second);
					mapSendData.erase(map_it);
					cout<<__FILE__<<" "<<__LINE__<<" snd "<<++cnt<<endl;
				}
//				usleep(1);
			}
		}
		//cout<<__FILE__<<" "<<__LINE__<<" "<< ++cnt2<<endl;
	}
}
