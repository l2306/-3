// 
// @file ThreadPool.h
//线程池的实现 , 是个管理线程 , 负责调用每个线程之间的互相调用的关系
// @author
// @date
// 

#ifndef __THREADPOOL__
#define __THREADPOOL__

#include "comm/Log.h"
#include "comm/Queue.h"

#include "thread_listen.h"
#include "thread_SndMsg.h"
#include "thread_RcvMsg.h"
#include "zz_worker.h"

class z_threadListen;
class z_threadSndMsg;
class z_threadRcvMsg;
class z_Worker;

class z_ThreadPool : public z_Thread
{
	public:
		//	构造函数
		z_ThreadPool(unsigned int num , z_Worker *m_work);

		//	析构函数
		~z_ThreadPool();

		//	服务器帮定端口
		//		nPort 帮定端口
		//	true表示成功 , false表示失败
		bool Bind(unsigned int nPort);

		//	主线程
		void Run();

		// 	填加socket到队列中
		//		nSocket 套接口
		//	true 表示成功 , false 表示失败
		bool pushSocket(unsigned int nSocket);

		//	从队列中取套接字
		//		nSocket 取出的套接字存放在nSocket中
		//	true 表示成功 , false 表示失败
		bool popSocket(int &nSocket);

		//	从z_Data->z_threadRcvMsg->z_ThreadPool->z_Worker 回掉
		//		pStr 客户发的字符串
		//	nSocket 接受客户连接的套接字
		void recvMessage(void *pStr , int nSocket);

		//	发送数据 从testPool->z_Worker->z_ThreadPool->z_threadSndMsg->z_Data
		//		pStr 数据
		//		nSocket 套接口
		//	return 
		int sendMessage(int nSocket , const void *pStr);

		//	接收数据 将接收数据 返回给 work
		void rcvMsg(tlv_t *pTLV, int nSocket);

		//	发送数据 将发送数据对 存入发送线程队列
		int sndMsg(int nSocket, tlv_t *pTLV);

	private:

		z_Worker *m_worker;

		//	监听线程
		z_threadListen *m_listenThread;

		//	发送消息线程
		z_threadSndMsg *m_threadSndMsg;

		//	存放socket队列	过渡队列 用于 listen -> pool -> recv
		z_Queue<int> m_sockQueue;

		//	存放空闲工作线程队列		//epoll监听套接字 不满峰值的 线程
		z_Queue<z_threadRcvMsg*> m_idleRecvMessThreadQueue;

		//	存放忙碌工作线程队列		//epoll 中监听套接字 达到 设定峰值   的线程
		z_Queue<z_threadRcvMsg*> m_busyRecvMessThreadQueue;
		

		//	每个RecvMessThread线程中最大用户数
		static const int maxCounter = 1000;

		//	如果线程不够用新增加的线程
		static const int addTaskThread = 1;

};

#endif
