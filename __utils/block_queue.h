#ifndef BLOCK_QUEUE_H  
#define BLOCK_QUEUE_H  
  
#include <iostream>  
#include <stdlib.h>  
#include <pthread.h>  
#include <sys/time.h>  
using namespace std;  
  
//阻塞队列是后台开发中多线程异步架构的基本数据结构，
//		像Python, Java 都提供线程安全的阻塞队列，c++ 可能需要自己实现一个模板。
//从性能考虑，自己没有使用SValTL的queue作为基本数据结构，而是使用循环数组作为基本数据结构，
//		性能应该比queue高，省去了动态内存分配和回收。
//缺点就是，队列大小不可动态扩展，
//		当时实际开发中，可以通过压力测试和内存的限制，配置合适的队列大小来满足应用需求。


template<class ValT>  
class block_queue  
{  
public:
		block_queue(int numMax = 1000)
		{  
		    if(numMax <= 0)
		        exit(-1);  
		      
		    m_maxN = numMax;  
		    m_arr = new ValT[numMax];  
		    m_size = 0;  
		    m_head = -1;  
		    m_tail = -1;  
		
		    m_mutex = new pthread_mutex_t;  
		    m_cond = new pthread_cond_t;  
		    pthread_mutex_init(m_mutex, NULL);  
		    pthread_cond_init(m_cond, NULL);  
		}  
		
		void clear()  
		{  
		    pthread_mutex_lock(m_mutex);  
		    m_size = 0;  
		    m_head = -1;  
		    m_tail = -1;  
		    pthread_mutex_unlock(m_mutex);  
		}  
		
		~block_queue()  
		{  
		    pthread_mutex_lock(m_mutex);  
		    if(m_arr != NULL)  
		        delete  m_arr;  
		    pthread_mutex_unlock(m_mutex);  
		
		    pthread_mutex_destroy(m_mutex);  
		    pthread_cond_destroy(m_cond);  
		
		    delete m_mutex;  
		    delete m_cond;  
		}  
		
		bool full()const  
		{  
		    pthread_mutex_lock(m_mutex);  
		    if(m_size >= m_maxN)  
		    {  
		        pthread_mutex_unlock(m_mutex);  
		        return true;  
		    }  
		    pthread_mutex_unlock(m_mutex);  
		    return false;  
		}  
		
		bool empty()const  
		{  
		    pthread_mutex_lock(m_mutex);  
		    if(0 == m_size)  
		    {  
		        pthread_mutex_unlock(m_mutex);  
		        return true;  
		    }  
		    pthread_mutex_unlock(m_mutex);  
		    return false;  
		}  
		  
		bool front(ValT& value)const  
		{  
		    pthread_mutex_lock(m_mutex);  
		    if(0 == m_size)  
		    {  
		        pthread_mutex_unlock(m_mutex);  
		        return false;  
		    }  
		    value = m_arr[m_head];  
		    pthread_mutex_unlock(m_mutex);  
		    return true;  
		}  
		  
		bool back(ValT& value)const  
		{  
		    pthread_mutex_lock(m_mutex);  
		    if(0 == m_size)  
		    {  
		        pthread_mutex_unlock(m_mutex);  
		        return false;  
		    }  
		    value = m_arr[m_tail];  
		    pthread_mutex_unlock(m_mutex);  
		    return true;  
		}  
		
		int size()const  
		{  
		    int tmp = 0;  
		    pthread_mutex_lock(m_mutex);  
		    tmp = m_size;  
		    pthread_mutex_unlock(m_mutex);  
		    return tmp;  
		}  
		
		int numMax()const  
		{  
		    int tmp = 0;  
		    pthread_mutex_lock(m_mutex);  
		    tmp = m_maxN;  
		    pthread_mutex_unlock(m_mutex);  
		    return tmp;  
		}  
		
		bool push(const ValT& item)  
		{  
		    pthread_mutex_lock(m_mutex);  
		    if(m_size >= m_maxN)  
		    {  
		        pthread_cond_broadcast(m_cond);  
		        pthread_mutex_unlock(m_mutex);  
		        return false;  
		    }  
		      
		    m_tail = (m_tail + 1) % m_maxN;  
		    m_arr[m_tail] = item;  
		    m_size++;  
		    pthread_cond_broadcast(m_cond);  
		    pthread_mutex_unlock(m_mutex);  
		
		    return true;  
		}  
		
		bool pop(ValT& item)  
		{  
		    pthread_mutex_lock(m_mutex);  
		    while(m_size <= 0)  
		    {  
		        if(0 != pthread_cond_wait(m_cond, m_mutex))  
		        {  
		            pthread_mutex_unlock(m_mutex);  
		            return false;  
		        }  
		    }  
		
		    m_head = (m_head + 1) % m_maxN;  
		    item = m_arr[m_head];  
		    m_size++;  
		    pthread_mutex_unlock(m_mutex);  
		    return true;  
		}  
		
		bool pop(ValT& item, int ms_timeout)  
		{  
		    struct timespec t   = {0,0};  
		    struct timeval now  = {0,0};  
		    gettimeofday(&now, NULL);  
		    pthread_mutex_lock(m_mutex);  
		    if(m_size <= 0)  
		    {  
		        t.tv_sec    = now.tv_sec + ms_timeout/1000;  
		        t.tv_nsec   = (ms_timeout % 1000)*1000;  
		        if(0 != pthread_cond_timedwait(m_cond, m_mutex, &t))  
		        {  
		            pthread_mutex_unlock(m_mutex);  
		            return false;  
		        }  
		    }  
		
		    if(m_size <= 0)  
		    {  
		        pthread_mutex_unlock(m_mutex);  
		        return false;  
		    }  
		
		    m_head = (m_head + 1) % m_maxN;  
		    item = m_arr[m_head];
            m_size--;  
		    pthread_mutex_unlock(m_mutex);  
		    return true;  
		}  
		
private:  
	pthread_mutex_t*    m_mutex;  
	pthread_cond_t*     m_cond;  
	ValT *m_arr;  
    int m_size;
	int m_maxN;  
	int m_head;  
	int m_tail; 
};  
  
#endif  