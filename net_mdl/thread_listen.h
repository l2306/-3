// 
//服务器监听线程
// @author
// @date
// 

#ifndef __THREAD_LISTEN__
#define __THREAD_LISTEN__

#include "define.h"
#include "../__utils/_thread.h"
#include "../__utils/zz_socketutil.h"

class z_ThreadPool;

class z_threadListen : public z_Thread
{
	public:
		z_threadListen(z_ThreadPool *pool);		//	构造函数
		virtual ~z_threadListen();				//	析构函数
		void Run();
		bool Bind(unsigned short usPort);		//	服务器帮定端口
	private:
		//int 					_sockfd;			//	套接字
		int 					epfd;				//	epoll_create 返回文件描述符
		struct epoll_event 	events[256];		//  监听事件
		z_ThreadPool 			*_threadPool;
};

#endif
