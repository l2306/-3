// 
// @file ListenThread.h
//服务器监听线程
// @author
// @date
// 

#ifndef __G_LISTENTHREAD__
#define __G_LISTENTHREAD__

#include "comm/Log.h"
#include "comm/Lock.h"
#include "comm/Thread.h"
#include "comm/zz_socketutil.h"
#include "ThreadPool.h"

class z_ThreadPool;

class z_threadListen : public z_Thread
{

	public:
		//	构造函数
		z_threadListen(z_ThreadPool *pool);

		//	析构函数
		virtual ~z_threadListen();

		//	
		void Run();

		//	服务器帮定端口
		//		nPort 帮定端口
		// true表示成功 , false表示失败
		bool Bind(unsigned short usPort);

		//	如果有client连接得到套接字
		//		nSocket 返回套接字
		//	true 表示获得套接字成功 , false 表示获得套接字失败
		bool Listen(int &nSocket);

	private:
	
		//	套接字
		int m_sockfd;

		//	epoll_create 返回文件描述符
		int epfd;

		struct epoll_event events[256];


		z_ThreadPool *m_threadPool;

};

#endif
