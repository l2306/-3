
#include "ThreadPool.h"
#include "zz_server.h"

z_ThreadPool::z_ThreadPool(unsigned int numRcvTh , z_Worker *m_work) : _worker(m_work)
{
	//启动监听线程
	_listenThread = new z_threadListen(this);
	_listenThread->start();		

	//发送消息线程
	_threadSndMsg = new z_threadSndMsg();
	_threadSndMsg->start();

	// 启动处理client发送信息线程 , 收消息线程
	for (int i = 0; i<numRcvTh; i++)
	{
		z_threadRcvMsg *m_threadRcvMsg = new z_threadRcvMsg(this);
		_queIdleThread_RcvMsg.push(m_threadRcvMsg);
		m_threadRcvMsg->start(); 
	}

	start();   //线程池自己启动
}

z_ThreadPool::~z_ThreadPool()
{
	if(_listenThread)
	{
		delete _listenThread;
		_listenThread = NULL;
	}
	if(_threadSndMsg)
	{
		delete _threadSndMsg;
		_threadSndMsg = NULL;
	}

	_sockQueue.clear();

	z_threadRcvMsg *tmp_threadRcvMsg;
	while(_queIdleThread_RcvMsg.pop(tmp_threadRcvMsg))
	{
		delete tmp_threadRcvMsg;
		tmp_threadRcvMsg = NULL;
	}
	while(_queBusyThread_RcvMsg.pop(tmp_threadRcvMsg))
	{
		delete tmp_threadRcvMsg;
		tmp_threadRcvMsg = NULL;
	}
}

bool z_ThreadPool::bind(unsigned int nPort)
{
	return _listenThread->Bind(nPort);
}

int z_ThreadPool::sndMsg(int nSocket, tlv_t *pTLV)
{
	_threadSndMsg->sndMsg(nSocket, pTLV);
}

bool z_ThreadPool::pushSocket(int nSocket)
{
	return _sockQueue.push(nSocket);
}

bool z_ThreadPool::popSocket(int &nSocket)
{
	return _sockQueue.pop(nSocket);
}

void z_ThreadPool::rcvMsg(tlv_t *pTLV, int nSocket)
{
	_worker->rcvMsg(pTLV, nSocket);
}

void z_ThreadPool::Run()
{
	int nSocket;  
	z_threadRcvMsg *m_threadRcvMsg;		//存储临时对象
	while(1)
	{
	AddSockSuccess:		
		suspend();		//等待ListenThread 发信号
		while(popSocket(nSocket))     //必须把存放socket队列中的套接口全部取出
		{ 
				_threadSndMsg->addEpoll(nSocket);

				int idle = _queIdleThread_RcvMsg.size();
				for (size_t i = 0; i < idle; i++)
				{
					if (_queIdleThread_RcvMsg.pop(m_threadRcvMsg))
					{
						if (m_threadRcvMsg->getCounter() < 500)
						{
							m_threadRcvMsg->addSocket(nSocket);		//添加
							m_threadRcvMsg->continues();				//唤醒
							_queIdleThread_RcvMsg.push(m_threadRcvMsg);
							goto AddSockSuccess;
						}
						else
						{
							_queBusyThread_RcvMsg.push(m_threadRcvMsg);
							continue;
						}
					}
				}
//				debug_output("nSocket = %d\n", nSocket);
				//空闲队列没有线程   则创建
				for (int i = 0; i < addTaskThread; i++)
				{
						z_threadRcvMsg *m_threadRcvMsg = new z_threadRcvMsg(this);
						_queIdleThread_RcvMsg.push(m_threadRcvMsg);
						m_threadRcvMsg->addSocket(nSocket);		//添加
						m_threadRcvMsg->start();
				}
		}
	}
}
