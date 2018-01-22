
#include "ThreadPool.h"

z_ThreadPool::z_ThreadPool(unsigned int numRcvTh , z_Worker *m_work) : m_worker(m_work)
{
	//启动监听线程
	m_listenThread = new z_threadListen(this);
	m_listenThread->Start();		

	//发送消息线程
	m_threadSndMsg = new z_threadSndMsg();
	m_threadSndMsg->Start();

	// 启动处理client发送信息线程 , 收消息线程
	for (int i = 0; i<numRcvTh; i++)
	{
		z_threadRcvMsg *m_threadRcvMsg = new z_threadRcvMsg(this);
		m_idleRecvMessThreadQueue.push(m_threadRcvMsg);
		m_threadRcvMsg->Start();        
	}

	Start();   //线程池自己启动
}

z_ThreadPool::~z_ThreadPool()
{
	if(m_listenThread)
	{
		delete m_listenThread;
		m_listenThread = NULL;
	}
	if(m_threadSndMsg)
	{
		delete m_threadSndMsg;
		m_threadSndMsg = NULL;
	}

	m_sockQueue.clear();

	z_threadRcvMsg *m_threadRcvMsg;
	while(m_idleRecvMessThreadQueue.pop(m_threadRcvMsg))
	{
		delete m_threadRcvMsg;
		m_threadRcvMsg = NULL;
	}
	while(m_busyRecvMessThreadQueue.pop(m_threadRcvMsg))
	{
		delete m_threadRcvMsg;
		m_threadRcvMsg = NULL;
	}
}

bool z_ThreadPool::Bind(unsigned int nPort)
{
	return m_listenThread->Bind(nPort);
}

int z_ThreadPool::sndMsg(int nSocket, tlv_t *pTLV)
{
	m_threadSndMsg->sndMsg(nSocket, pTLV);
}

bool z_ThreadPool::pushSocket(unsigned int nSocket)
{
	return m_sockQueue.push(nSocket);
}

bool z_ThreadPool::popSocket(int &nSocket)
{
	return m_sockQueue.pop(nSocket);
}

void z_ThreadPool::rcvMsg(tlv_t *pTLV, int nSocket)
{
	m_worker->rcvMsg(pTLV, nSocket);
}

void z_ThreadPool::Run()
{
	int nSocket;  
	z_threadRcvMsg *m_threadRcvMsg;		//存储临时对象
	while(1)
	{
	AddSockSuccess:		
		pause();		//等待ListenThread 发信号
		while(popSocket(nSocket))     //必须把存放socket队列中的套接口全部取出
		{ 
				m_threadSndMsg->addEpoll(nSocket);

				int idle = m_idleRecvMessThreadQueue.size();
				for (size_t i = 0; i < idle; i++)
				{
					if (m_idleRecvMessThreadQueue.pop(m_threadRcvMsg))
					{
						if (m_threadRcvMsg->getCounter() < 500)
						{
							m_threadRcvMsg->addSocket(nSocket);		//添加
							m_threadRcvMsg->continues();			//唤醒
							m_idleRecvMessThreadQueue.push(m_threadRcvMsg);
							goto AddSockSuccess;
						}
						else
						{
							m_busyRecvMessThreadQueue.push(m_threadRcvMsg);
							continue;
						}
					}
				}
				debug_output("nSocket = %d\n", nSocket);
				//空闲队列没有线程   则创建
				for (int i = 0; i < addTaskThread; i++)
				{
						z_threadRcvMsg *m_threadRcvMsg = new z_threadRcvMsg(this);
						m_idleRecvMessThreadQueue.push(m_threadRcvMsg);
						m_threadRcvMsg->addSocket(nSocket);		//添加
						m_threadRcvMsg->Start();
				}
		}
//添加线程有问题
				////从不满队列中获得
				//if(m_idleRecvMessThreadQueue.getFront(m_threadRcvMsg)) 
				//{
				//	if(m_threadRcvMsg->getCounter() < maxCounter)  
				//	{
				//		m_threadRcvMsg->addSocket(nSocket);		//添加
				//		m_threadRcvMsg->continues();			// 唤醒 
				//		break;
				//	}
				//	else
				//	{
				//		if(m_idleRecvMessThreadQueue.pop(m_threadRcvMsg))
				//		{
				//			m_busyRecvMessThreadQueue.push(m_threadRcvMsg);
				//		}
				//		else
				//		{
				//			continue;
				//		}
				//	}
				//}
				//else
				//{			//空闲队列没有线程   则创建
				//	for (int i = 0; i<addTaskThread; i++)
				//	{
				//		z_threadRcvMsg *m_threadRcvMsg = new z_threadRcvMsg(this);
				//		m_idleRecvMessThreadQueue.push(m_threadRcvMsg);
				//		m_threadRcvMsg->Start();
				//	}
				//}
	}
}
