
#include "Data.h"

z_Data::z_Data(z_threadRcvMsg *recvMessThread)
	:	m_recvMessThread(recvMessThread)
{
}

z_Data::z_Data()
{
}

z_Data::~z_Data()
{
}

int z_Data::sendn(int nSocket , const void *pStr , unsigned int nLen)
{
	int n = nLen , nRet;
	char *str = (char*)pStr;
	while(n > 0)
	{
		nRet = send(nSocket , str , n , MSG_NOSIGNAL);
		if(nRet <= 0)
		{
			if(errno == EINTR)
			{
				continue;
			}
			return -1;
		}
		n -= nRet;
		str += nRet;
	}
	return (nLen - n);
}

int z_Data::recvn(int nSocket , void *pStr , unsigned int nLen)
{
	int n = nLen;
	char *str = (char*)pStr;

	while(n > 0)
	{
		int nRet = recv(nSocket , str , n , MSG_NOSIGNAL);
		if(nRet <= 0)
		{
			if(errno == EINTR)
			{
				continue;
			}
			break;
		}
		n -= nRet;
		str += nRet;
	}
	return (nLen - n);
}

int z_Data::sndData(int nSocket, tlv_t *pTLV)
{
	unsigned int sizeBuffer = sizeof(int) + sizeof(tlv_t) + pTLV->len;

	int nSize = htonl(sizeBuffer);		//	转换为网络字节序  以便跨平台
	pTLV->len = htons(pTLV->len);

	int nRet = send(nSocket, &nSize, sizeof(int) , MSG_NOSIGNAL);
	if (nRet <= 0){
		return -1;
	}
	nRet = sendn(nSocket, pTLV, sizeBuffer);
	if (nRet <= 0){
		return -1;
	}
	free(pTLV);

	return nRet;
}

bool z_Data::rcvData(int nSocket)
{

	unsigned int uiBufLen =0;
	int nSize = recv(nSocket, &uiBufLen, sizeof(int), MSG_NOSIGNAL);
	if (nSize <= 0){
		return false;
	}

	int sizeBuffer = ntohl(uiBufLen);	// 本包内容长度
	tlv_t* pTLV = (tlv_t*)malloc( sizeBuffer );

	recvn(nSocket, pTLV, sizeBuffer);
	pTLV->len = ntohs( pTLV->len );

	pair_SockData tmp={ nSocket , pTLV };
	m_recvMessThread->pushData( tmp );          //放到数据池队列中
//static int cnt = 0;
//cout << "---"<< __FILE__<<ends<< __FUNCTION__ <<ends<<" cnt " << ++cnt << endl;
	return true;
}

//		MSG_NOSIGNAL 禁止 send() 函数向系统发送异常消息。
//当连接断开，还发送数据的时候，不仅 send() 的返回值会有反映，而且还会向系统发送一个异常消息，
//		如果不作处理，系统会出 BrokePipe，程序会退出
