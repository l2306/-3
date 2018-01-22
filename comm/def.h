 
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

//为了服务器接收到client的信息把数据和client的套接字连接到一起
struct z_DataSocket
{
	int nSocket;
	void *pStr;
};

/*
typedef struct TLV{
	uint8_t tag;
	uint16_t len;
	char value[0];		//变长数组  c99柔性数组 
}__attribute__((packed)) tlv_t;	//TLV 编解码格式
*/
typedef struct TLV{
	uint8_t tag;
	uint16_t len;
	char value[0];		//变长数组  c99柔性数组 
}tlv_t;					//TLV 编解码格式


struct pair_SockData{
	int sock;
	tlv_t* pTLV;
	pair_SockData& operator=(pair_SockData& value)
	{
	    sock = value.sock;
        pTLV = value.pTLV;
        return *this;
	}
};

#endif
