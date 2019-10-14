#ifndef __Z_SOCKUTIL__
#define __Z_SOCKUTIL__
// 
//	设置socket属性

//cat /proc/sys/fs/file-max

#include <stdlib.h>     
#include <fcntl.h>     
#include <errno.h>     
#include <sys/socket.h>     
#include <netinet/tcp.h>     
#include <netinet/in.h>     
#include <netdb.h>     
#include <arpa/inet.h>  

//设置 socket 保活心跳 
//	fd		套接字
// 	alive		打开关闭 1:开启保活  0:关闭保活
//	idle		秒钟无数据，触发保活机制，发送保活包
//	cnt			连续３次没收到保活包，视为连接失效   
//	intvl 		触发保活后，发包间隔时间
//回 0设置成功
int socket_set_keepalive(int fd , int alive , int idle , int cnt , int intvl);

//设置 socket 发送，接受的缓存区大小
//	fd 套接字
//回 0设置成功
int setSockBuffSize(int fd_sock , int len_sndbuf ,int len_rcvbuf);

bool setNonBlock(int fd_sock);

bool isCanRcv(int fd_socket, long sec, long usec);

/*
在默认的情况下，TCP连接是没有保活的心跳的。
    即，当一个TCP的socket，客户端与服务端谁也不发送数据，会一直保持着连接。	
    （只有socket的close或程序结束时  发送FIN时才会）
    这其中如果有一方异常掉线（断网线 或 掉电），另一端永远也不可能知道。
    这对于一些服务型的程序来说，将是灾难性的后果。
*/

/*
Linux环境下的TCP Keepalive参数设置
		没有手动设置，是采用的系统默认值
		如果是Linux操作系统，这三个值分别为
		# cat /proc/sys/net/ipv4/tcp_keepalive_time     
		7200     
		# cat /proc/sys/net/ipv4/tcp_keepalive_intvl     
		75     
		# cat /proc/sys/net/ipv4/tcp_keepalive_probes     
		9    
　　在Linux系统下，如果对于TCP的socket启用了Keepalive选项，
        则会在7200秒（即两个小时）没有数据后，发起KEEPALIVE报文
	如果没有回应，则会在75秒后再次重试。
        如果重试9次均失败，则认定连接已经失效。TCP的读取操作，将返回0。
	对大多数应用  前两个值有大
*/

#endif
