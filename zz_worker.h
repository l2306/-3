// 
// @file Worker.h
//用户接口类
// @author 
// @date 
// 

#ifndef __WORKER__
#define __WORKER__

#include "comm/Log.h"

#include "ThreadPool.h"

//抽象类
class z_Worker
{
	public:
		//	构造函数
		z_Worker(unsigned int num);

		//	析构函数
		~z_Worker();

		//	服务器帮定端口
		//		nPort 帮定端口
		//	true表示成功 , false表示失败
		bool Bind(unsigned int nPort);

		// 接收数据接口 需实现
		virtual void rcvMsg(tlv_t *pStr, int nSocket) = 0;

		// 发送数据接口	不能重写  可以用关键词吗??
		int sndMsg(int nSocket, tlv_t *pTLV);

	protected:
		
	private:
		z_ThreadPool *g_threadPool;
};

#endif
