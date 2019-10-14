// 
//		服务器框架基类
// @author
// @date
// 

#ifndef __Z_SERVER__
#define __Z_SERVER__

#include "define.h"

class z_ThreadPool;

//抽象服务端		把服务端通用操作封装
class z_Server
{
	public:
		virtual ~z_Server();			//  析构函数
		virtual bool init();			//	初始化服务器程序
		virtual void final() = 0;		//	结束服务器程序回收资源
		static void delself()			// 追加 
		{
			if(instance)
				delete instance;
			instance = NULL;
		}

	protected:
		z_Server();					//	构造函数

	private:
		static z_Server *instance ;

};

//抽象类		实际接受数据的类
class z_Worker
{
	public:
		z_Worker(unsigned int num);
		~z_Worker();
		bool bind(unsigned int nPort);							//	服务器绑定端口
		int sndMsg(int nSocket, tlv_t *pTLV);					// 发送数据接口	不能重写
		virtual void rcvMsg(tlv_t *pStr, int nSocket) = 0;		// 接收数据接口 需实现

	protected:
		
	private:
		z_ThreadPool *_threadPool;
};

#endif

