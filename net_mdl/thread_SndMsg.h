// 
//发送消息线程 server->client
// @author
// @date
// 

#ifndef __THREAD_SNDMSG__
#define __THREAD_SNDMSG__

#include "define.h"
#include "../__utils/_queue.h"
#include "../__utils/_thread.h"
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
		int epfd;											//	epoll_create 返回文件描述符
		struct epoll_event event[1000] , ev;
		z_Data *_data;										//	数据发送对象
		typedef pair<int, tlv_t*> pair_mapSendData;		//	发送数据结构体定义
		multimap<int, tlv_t*> mapSendData;				//	存放套接字和数据multimap
};

#endif
