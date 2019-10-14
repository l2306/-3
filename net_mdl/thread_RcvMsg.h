// 
//任务类 , 接受client发的消息进行处理
// @author
// @date
// 

#ifndef __THREAD_RCVMSG__
#define __THREAD_RCVMSG__

#include "define.h"
#include "../__utils/zz_socketutil.h"
#include "../__utils/_queue.h"
#include "../__utils/_thread.h"
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
		void Run();						//	主线程运行
		void addSocket(int nSocket);		//	填加套接字
		//void delSocket(int nSocket);	//	删除套接字
		unsigned int getCounter();		//	获得连接的客户端数目
		bool pushData(pair_SockData);

	private:
		int epfd;										//	epoll_create 返回文件描述符
		struct epoll_event events[g_numRcvMsgEV];	//
		unsigned int counter;							//	记录接受客户端数目
		z_ThreadPool *g_threadPool;					// 线程池对象
		z_Data *_data;									// 数据处理
		z_Queue<pair_SockData> que_tlvBuffer;		//	存放数据的队列	接收 data -> 本 -> thread
};

#endif
