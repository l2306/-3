// 
// @file SendMessThread.h
//发送消息线程 server->client
// @author
// @date
// 

#ifndef __SENDMESSTHREAD__
#define __SENDMESSTHREAD__

#include "comm/def.h"
#include "comm/Log.h"
#include "comm/Queue.h"
#include "comm/Thread.h"
#include "Data.h"
#include "ThreadPool.h"

class z_Data;

class z_threadSndMsg : public z_Thread
{
	public:

		z_threadSndMsg();

		~z_threadSndMsg();

		void Run();

		void addEpoll(int nSocket);

		void modEpoll(int nSocket);

		void delEpoll(int nSocket);

		int sndMsg(int nSocket, tlv_t *pStr);

	private:
		//	epoll_create 返回文件描述符
		int epfd;

		struct epoll_event event[1000] , ev;

		//	数据发送对象
		z_Data *m_data;

		//	发送数据结构体定义
		typedef pair<int, tlv_t*> pair_mapSendData;

		//	存放套接字和数据multimap
		multimap<int, tlv_t*> mapSendData;

};

#endif
