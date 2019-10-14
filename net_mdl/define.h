 
#ifndef __DEF__
#define __DEF__

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <strings.h>


#include <iostream>
#include <map>
#include <deque>

using namespace std;

typedef unsigned long ULONG;

#pragma pack(1)

//为了服务器接收到client的信息
//	把数据和client的套接字连接到一起
struct z_DataSocket
{
	int		nSocket;
	void	*pStr;
};

/*
typedef struct TLV{
	uint8_t 		tag;
	uint16_t 		len;
	char 			value[0];				//变长数组  c99柔性数组 
}__attribute__((packed)) tlv_t;			//TLV 编解码格式
*/
typedef struct TLV{
	uint8_t	tag;
	uint16_t	len;
	char		value[0];		//变长数组  c99柔性数组 
}tlv_t;						//TLV 编解码格式


struct pair_SockData{
	int		sock;
	tlv_t*	pTLV;
	pair_SockData& operator=(pair_SockData& value)
	{
		sock = value.sock;
		pTLV = value.pTLV;
		return *this;
	}
};

class z_Data
{
public:		
	bool rcvData(int fd, tlv_t** ppTLV)//	接收数据并存到缓冲池中
            {
            
                unsigned int uiBufLen =0;
                int nSize = recv(fd, &uiBufLen, sizeof(int), MSG_NOSIGNAL);
                if (nSize <= 0){
                    return false;
                }
            
                int sizeBuffer = ntohl(uiBufLen);				// 本包内容长度
                *ppTLV = (tlv_t*)malloc( sizeBuffer );
            
                recvn(fd, *ppTLV, sizeBuffer);
                (*ppTLV)->len = ntohs( (*ppTLV)->len );
                return true;
            }
	int sndData(int fd, tlv_t *pTLV)	//	发送数据
            {
                unsigned int sizeBuffer = sizeof(int) + sizeof(tlv_t) + pTLV->len;
            
                int nSize = htonl(sizeBuffer);				//	转换为网络字节序  以便跨平台
                pTLV->len = htons(pTLV->len);
            
                int nRet = send(fd, &nSize, sizeof(int) , MSG_NOSIGNAL);
                if (nRet <= 0){
                    return -1;
                }
                nRet = sendn(fd, pTLV, sizeBuffer);
                if (nRet <= 0){
                    return -1;
                }
                free(pTLV);
                return nRet;
            }

private:
	int sendn(int fd , const void *pBuf , unsigned int nLen)		//	向套接口发送数据
            {
                int n = nLen , nRet;
                char *buf = (char*)pBuf;
                while(n > 0)
                {
                    nRet = send(fd , buf , n , MSG_NOSIGNAL);
                    if(nRet <= 0)
                    {
                        if(errno == EINTR)
                        {
                            continue;
                        }
                        return -1;
                    }
                    n -= nRet;
                    buf += nRet;
                }
                return (nLen - n);
            }
	int recvn(int fd , void *pBuf , unsigned int nLen)			//	从套接口接受数据
		{
			int n = nLen;
			char *buf = (char*)pBuf;
            
			while(n > 0)
			{
				int nRet = recv(fd , buf , n , MSG_NOSIGNAL);
				if(nRet <= 0)
				{
					if(errno == EINTR)
						continue;
					break;
				}
				n -= nRet;
				buf += nRet;
			}
			return (nLen - n);
		}
};
//		MSG_NOSIGNAL 禁止 send() 函数向系统发送异常消息。
//当连接断开，还发送数据的时候，不仅 send() 的返回值会有反映，而且还会向系统发送一个异常消息，
//		如果不作处理，系统会出 BrokePipe，程序会退出

#endif

