// 
// @file Data.h
//套接字发送接收的封装	tcp单只能 1500左右  实现封装 不必关心单次
// @author guozhiming
// @date 2007-05-21
// 

#ifndef __DATA__
#define __DATA__

#include "comm/def.h"
#include "thread_RcvMsg.h"
#include "thread_SndMsg.h"

class z_threadRcvMsg;
class z_threadSndMsg;

class z_Data
{
	public:
		//	构造函数
		z_Data(z_threadRcvMsg *recvMessThread);

		z_Data();

		//	析构函数
		~z_Data();

		//	接收数据并存到缓冲池中
		//		nSocket	套接字 
		//	收到数据返回true , 如果client关闭返回false 
		bool recvData(int nSocket);

		//	接收数据并存到缓冲池中
		//		nSocket	套接字 
		//	收到数据 返回true ,client关闭 返回false 
		bool rcvData(int nSocket);

		//	发送数据
		//		nSocket 套接字
		//		pTLV 数据包
		// 
		int sndData(int nSocket, tlv_t *pTLV);


	private:

		//	发送数据
		//		nSocket 	套接字
		//		ptr      发送数据
		//		nLen     数据长度
		//	发送数据长度
		int sendn(int nSocket , const void *pStr , unsigned int nLen);

		//	从套接口接受数据
		//		nSocket 	套接口
		//		pStr     存放数据
		//		nLen     长度
		//	返回接受数据长度
		int recvn(int nSocket , void *pStr , unsigned int nLen);

		z_threadRcvMsg *m_recvMessThread;

};

#endif
