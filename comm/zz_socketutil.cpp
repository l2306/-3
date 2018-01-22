
#include "zz_socketutil.h"


bool setNonBlock(int sockfd)
{
	int opts = fcntl(sockfd , F_GETFL);
	if(-1 == opts)
	{
		debug_output("%s\n" , "fcntl F_GETFL is faild");
		return false;
	}

	opts = opts | O_NONBLOCK;
	if(fcntl(sockfd , F_SETFL , opts) < 0)
	{
		debug_output("%s\n" , "fcntl F_SETFL is faild");
		return false;
	}
	return true;
}

int socket_set_keepalive (int fd , int alive , int idle, int cnt , int intvl)     
{     
	int ret, error, flag		;//, alive, idle, cnt, intv;     
	
	/* Set: use keepalive on fd */        
	//const void *可以接受const char *型的参数，为跨平台编译考虑    
	if (setsockopt (fd, SOL_SOCKET, SO_KEEPALIVE, &alive, sizeof (alive)) != 0) 
	{     
//	log_warn ("Set keepalive error: %s.\n", strerror (errno));     
		return -1;     
	}     

	/* １０秒钟无数据，触发保活机制，发送保活包 */     
	if (setsockopt (fd, SOL_TCP, TCP_KEEPIDLE, &idle, sizeof (idle)) != 0)     
	{        
		return -1;     
	}     
	     
	/* 如果没有收到回应，则５秒钟后重发保活包 */     
	if (setsockopt (fd, SOL_TCP, TCP_KEEPINTVL, &intvl, sizeof (intvl)) != 0)     
	 {        
		return -1;     
	 }     
	     
	  /* 连续３次没收到保活包，视为连接失效 */     
	if (setsockopt (fd, SOL_TCP, TCP_KEEPCNT, &cnt, sizeof (cnt)) != 0)     
	{        
		return -1;     
	}
	     
	return 0;     
}

int setSockBuffSize(int sockfd , int sndbuflen ,int rcvbuflen)
{
	int ret; 
	ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuflen, sizeof(int)); 
	if ( ret < 0)    { 
		return -3; 
	} 
	ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sndbuflen, sizeof(int)); 
	if (ret < 0)    { 
		return -4; 
	 } 
	return 0; 
}

