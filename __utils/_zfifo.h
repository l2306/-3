
#ifndef _ZFIFO_H 
#define _ZFIFO_H 

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

//判断x是否是2的次方
#define is_power_of_2(x) 	((x) != 0 && (((x) & ((x) - 1)) == 0))
//取a和b中最小值
#define min(a, b) 			(((a) < (b)) ? (a) : (b))

//向上取整为 二的幂
uint32_t roundup_power_of_2(uint32_t num);

typedef struct zfifo{
    void         *buff;    	//缓冲区
    uint32_t     size;       	//大小
    uint32_t     in;         	//入口位置
    uint32_t     out;        	//出口位置
    pthread_mutex_t *mtx; 		//互斥锁 若不能保证任何时间最多只有一个读线程和写线程，必须使用该lock实施
}zfifo;

//@初始化缓冲区
//	buff:		缓冲区指针
//	size:		缓冲区大小
//	mtx: 		互斥锁
//@回:	成回ptr 
//		败回NULL
struct zfifo* zfifo_init(void *buff, uint32_t size, pthread_mutex_t *mtx);

//@初始化缓冲区	将大小自动升级为比自身大的最小2次幂
//	size:		缓冲区大小
//	mtx: 		互斥锁
//@回:	成回ptr 
//		败回NULL
struct zfifo* zfifo_init_2power(uint32_t size);

//@释放缓冲区
void zfifo_free(struct zfifo *ptr_zfifo);

//@初始化缓冲区
//	ptr_zfifo:	
//@回:	成回缓存区的大小	
uint32_t zfifo_len(const struct zfifo *ptr_zfifo);

//@从z_fifo中抓取size大小的数据到 buff
//	ptr_zfifo:	
//	buff:		
//	size:		抓取内容的大小
//@回:	成回 成功抓取的buff大小
uint32_t zfifo_get(struct zfifo *ptr_zfifo, void *buff, uint32_t size);

//@将buff中size大小的数据存放到buff
//	ptr_zfifo:	
//	buff:		
//	size:		存放内容的大小
//@回:	成回 成功存放的buff大小
uint32_t zfifo_put(struct zfifo *ptr_zfifo, void *buff, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif
