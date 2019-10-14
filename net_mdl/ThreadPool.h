// 
//线程池的实现 , 是个管理线程 , 负责调用每个线程之间的互相调用的关系
// @author
// @date
// 

#ifndef __THREADPOOL__
#define __THREADPOOL__

#include "../__utils/_queue.h"

#include "thread_listen.h"
#include "thread_SndMsg.h"
#include "thread_RcvMsg.h"

class z_threadListen;
class z_threadSndMsg;
class z_threadRcvMsg;
class z_Worker;

class z_ThreadPool : public z_Thread
{
	public:
		
		z_ThreadPool(unsigned int num , z_Worker *m_work);		//	构造函数
		~z_ThreadPool();											//	析构函数
		void Run();												//	主线程
		bool bind(unsigned int nPort);							//	服务器帮定端口	nPort 帮定端口
		bool pushSocket(int nSocket);							// 	填加socket到队列中
		bool popSocket(int &nSocket);							//	从队列中取套接字
		void recvMessage(void *pStr , int nSocket);				//	从z_Data->z_threadRcvMsg->z_ThreadPool->z_Worker 回掉
		int sendMessage(int nSocket , const void *pStr);		//	发送数据 从testPool->z_Worker->z_ThreadPool->z_threadSndMsg->z_Data
		void rcvMsg(tlv_t *pTLV, int nSocket);					//	接收数据 将接收数据 返回给 work
		int sndMsg(int nSocket, tlv_t *pTLV);					//	发送数据 将发送数据对 存入发送线程队列

	private:
		z_Worker *_worker;									
		z_threadListen *_listenThread;                      //	监听线程	
		z_threadSndMsg *_threadSndMsg;                      //	发送消息线程
		z_Queue<int> _sockQueue;                            //	存放socket队列	过渡队列 用于 listen -> pool -> recv
		z_Queue<z_threadRcvMsg*> _queIdleThread_RcvMsg;     //	存放空闲工作线程队列
		z_Queue<z_threadRcvMsg*> _queBusyThread_RcvMsg;     //	存放忙碌工作线程队列
		static const int max_cnt = 1000;                    //	每个RecvMessThread线程中最大用户数
		static const int addTaskThread = 1;                 //	如果线程不够用新增加的线程
};

#endif
