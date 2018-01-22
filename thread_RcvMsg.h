// 
// @file TaskThread.h
//任务类 , 接受client发的消息进行处理
// @author
// @date
// 

#ifndef __TASKTHREAD__
#define __TASKTHREAD__

#include "comm/def.h"
#include "comm/Log.h"
#include "comm/zz_socketutil.h"
#include "comm/Queue.h"
#include "comm/Thread.h"
#include "Data.h"
#include "ThreadPool.h"
//#include "ThrdPool/inc/xx_pool.h"

#define g_numRcvMsgEV	500

class z_ThreadPool;
class z_Data;

class z_threadRcvMsg : public z_Thread
{
	public:

		z_threadRcvMsg(z_ThreadPool *pool);

		~z_threadRcvMsg();

		//	主线程运行
		void Run();

		//	填加套接字
		//		nSocket 套接字
		void addSocket(int nSocket);

		//	删除套接字
		//		nSocket 套接字
		void delSocket(int nSocket);

		//	获得连接的客户端数目
		//		数目
		unsigned int getCounter();

		bool pushData(pair_SockData);

	private:

		//	epoll_create 返回文件描述符
		int epfd;

		struct epoll_event events[g_numRcvMsgEV];

		//	记录接受客户端数目
		unsigned int counter;

		// 线程池对象
		z_ThreadPool *g_threadPool;

		z_Data *g_data;		//数据处理

		//	存放数据的队列	接收 data -> 本 -> thread
		z_Queue<pair_SockData> que_tlvBuffer;

};

#endif
