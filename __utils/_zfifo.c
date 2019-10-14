
#include "zfifo.h"

//判断x是否是2的次方
#define is_power_of_2(x) 	((x) != 0 && (((x) & ((x) - 1)) == 0))
//取a和b中最小值
#define min(a, b) 		(((a) < (b)) ? (a) : (b))

//向上取整为 二的幂
uint32_t roundup_power_of_2(uint32_t num)
{
	if (num == 0)
		return 0;
	uint32_t pos = 0 , i;
	for (i = num; i != 0; i >>= 1)
		pos++;
	return (uint32_t)(1 << pos);
} 

//@初始化缓冲区
//	buff:		缓冲区指针
//	size:		缓冲区大小
//	ntx: 		互斥锁
//@回:	成回ptr 
//		败回NULL
struct zfifo* zfifo_init(void *buff, uint32_t size, pthread_mutex_t *mtx)
{
    assert(buff);
    struct zfifo *ptr_zfifo = NULL;
    if (!is_power_of_2(size))
    {
		fprintf(stderr,"size must be power of 2.\n");
        return ptr_zfifo;
    }
    ptr_zfifo = (struct zfifo *)malloc(sizeof(struct zfifo));
    if (!ptr_zfifo)
    {
        fprintf(stderr,"Failed to malloc memory,errno:%u,reason:%s", errno, strerror(errno));
        return ptr_zfifo;
    }
    memset(ptr_zfifo, 0, sizeof(struct zfifo));
    ptr_zfifo->buff = buff;
    ptr_zfifo->size = size;
    ptr_zfifo->in = 0;
    ptr_zfifo->out = 0;
    ptr_zfifo->mtx = mtx;
    return ptr_zfifo;
}

//给定size分配buffer和kfifo
struct zfifo *zfifo_init_2power(uint32_t size)
{
	unsigned char *buffer;
	struct zfifo *ret;
	if (!is_power_of_2(size)) {
		//BUG_ON(size > 0x80000000);
		size = roundup_power_of_2(size);
	}
	buffer = (unsigned char*)malloc(size);
	if (!buffer)
		return NULL;
		//return ERR_PTR(-ENOMEM);
	pthread_mutex_t *mtx = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	if (pthread_mutex_init(mtx, NULL) != 0)
	{
		fprintf(stderr, "Failed init mutex,errno:%u,reason:%s\n",
			errno, strerror(errno));
		return NULL;
	}
	ret = zfifo_init(buffer, size, mtx);
	if (!ret)
		free(buffer);

	return ret;
}

/*
//根据给定buffer创建一个kfifo
struct kfifo *kfifo_init(unsigned char *buffer, unsigned int size, gfp_t gfp_mask, spinlock_t *lock)
{
	struct kfifo *fifo;
	BUG_ON(!is_power_of_2(size));
	fifo = kmalloc(sizeof(struct kfifo), gfp_mask);
	if (!fifo)
		return ERR_PTR(-ENOMEM);
	fifo->buffer = buffer;
	fifo->size = size;
	fifo->in = fifo->out = 0;
	fifo->lock = lock;
	return fifo;
}

//给定size分配buffer和kfifo
struct kfifo *kfifo_alloc(unsigned int size, gfp_t gfp_mask, spinlock_t *lock)
{
	unsigned char *buffer;
	struct kfifo *ret;
	if (!is_power_of_2(size)) {
		BUG_ON(size > 0x80000000);
		size = roundup_power_of_2(size);
	}
	buffer = kmalloc(size, gfp_mask);
	if (!buffer)
		return ERR_PTR(-ENOMEM);
	ret = kfifo_init(buffer, size, gfp_mask, lock);
	if (IS_ERR(ret))
		kfree(buffer);
	return ret;
}
*/

//@释放缓冲区
void zfifo_free(struct zfifo *ptr_zfifo)
{
	if (ptr_zfifo)
	{
		if (ptr_zfifo->buff)
		{
			free(ptr_zfifo->buff);
			ptr_zfifo->buff = NULL;
		}
		free(ptr_zfifo);
		ptr_zfifo = NULL;
	}
}

//缓冲区的长度
uint32_t __zfifo_len(const struct zfifo *ptr_zfifo)
{
    return (ptr_zfifo->in - ptr_zfifo->out);
}

//从缓冲区中取数据
uint32_t __zfifo_get(struct zfifo *ptr_zfifo, void * buff, uint32_t size)
{
    assert(ptr_zfifo || buff);
    uint32_t len = 0;
    size  = min(size, ptr_zfifo->in - ptr_zfifo->out);        
    /* 首先 抓取相应的buff直到尾部 */
    len = min(size, ptr_zfifo->size - (ptr_zfifo->out & (ptr_zfifo->size - 1)));
    memcpy(buff, ptr_zfifo->buff + (ptr_zfifo->out & (ptr_zfifo->size - 1)), len);
    /* 然后 从头部开始抓取剩余部分 */
    memcpy(buff + len, ptr_zfifo->buff, size - len);
    ptr_zfifo->out += size;
    return size;
}
//向缓冲区中存放数据
uint32_t __zfifo_put(struct zfifo *ptr_zfifo, void *buff, uint32_t size)
{
    assert(ptr_zfifo || buff);
    uint32_t len = 0;
    size = min(size, ptr_zfifo->size - ptr_zfifo->in + ptr_zfifo->out);
    /* 首先 存放相应的buff直到尾部 */
    len  = min(size, ptr_zfifo->size - (ptr_zfifo->in & (ptr_zfifo->size - 1)));
    memcpy(ptr_zfifo->buff + (ptr_zfifo->in & (ptr_zfifo->size - 1)), buff, len);
    /* 然后 从头部开始存放剩余部分 */
    memcpy(ptr_zfifo->buff, buff + len, size - len);
    ptr_zfifo->in += size;
    return size;
}

//@初始化缓冲区
//	ptr_zfifo:	
//@回:	成回缓存区的大小	
uint32_t zfifo_len(const struct zfifo *ptr_zfifo)
{
    pthread_mutex_lock(ptr_zfifo->mtx);
    uint32_t len = __zfifo_len(ptr_zfifo);
    pthread_mutex_unlock(ptr_zfifo->mtx);
    return len;
}

//@从z_fifo中抓取size大小的数据到 buff
//	ptr_zfifo:	
//	buff:		
//	size:		抓取内容的大小
//@回:	成回 成功抓取的buff大小
uint32_t zfifo_get(struct zfifo *ptr_zfifo, void *buff, uint32_t size)
{
    pthread_mutex_lock(ptr_zfifo->mtx);
    uint32_t rtn= __zfifo_get(ptr_zfifo, buff, size);
    if (ptr_zfifo->in == ptr_zfifo->out)
		ptr_zfifo->in = ptr_zfifo->out = 0;
    pthread_mutex_unlock(ptr_zfifo->mtx);
    return rtn;
}

//@将buff中size大小的数据存放到buff
//	ptr_zfifo:	
//	buff:		
//	size:		存放内容的大小
//@回:	成回 成功存放的buff大小
uint32_t zfifo_put(struct zfifo *ptr_zfifo, void *buff, uint32_t size)
{
    pthread_mutex_lock(ptr_zfifo->mtx);
    uint32_t rtn = __zfifo_put(ptr_zfifo, buff, size);
    pthread_mutex_unlock(ptr_zfifo->mtx);
    return rtn;
}
