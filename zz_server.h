// 
// @file Server.h
//		服务器框架基类
// @author
// @date
// 

#ifndef __SERVER__
#define __SERVER__

#include "comm/def.h"
#include "comm/Log.h"

class z_Server
{
	public:

		// 析构函数
		virtual ~z_Server();
	
		//	初始化服务器程序字类需要继承这个函数
		//	
		virtual bool init();

		//	结束服务器程序回收资源
		virtual void final() = 0;

		//追加 
		static void delself()
		{
			if(instance)
				delete instance;
			instance = NULL;
		}

	protected:

		//	构造函数
		z_Server();

	private:
		static z_Server *instance ;

};

#endif

