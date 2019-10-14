
#ifndef _LCK_RW_
#define _LCK_RW_
//目的		简化加锁解锁代码	利用局部变量的构造、析构 使用模板

class Lock_RW;

class Lock1_RW;
class Lock2_RW;
class Lock3_RW;

template <class TT>
class lock_r;
template <class TT>
class lock_w;


////////////////////定义如下

template <class TT>
class lock_r{
public:
	lock_r(TT* rw):_rw(rw)	{_rw->lock_r();}
	~lock_r()				{_rw->free_r();}
private:
	TT* _rw;
};
template <class TT>
class lock_w{
public:
	lock_w(TT* rw):_rw(rw)	{_rw->lock_w();}
	~lock_w()				{_rw->free_w();}
private:
	TT* _rw;
};

#include <pthread.h> //多线程、互斥锁所需头文件
////	1，直接使用读写锁
class Lock_RW{
public:
	Lock_RW(){	pthread_rwlock_t  rwlock = PTHREAD_RWLOCK_INITIALIZER; };
	void lock_r()	{	pthread_rwlock_rdlock(&rwlock);	}
	void free_r()	{	pthread_rwlock_unlock(&rwlock);	}	//unlock
	void lock_w()	{	pthread_rwlock_wrlock(&rwlock);	}
	void free_w()	{	pthread_rwlock_unlock(&rwlock);	}	//unlock
private:	
	pthread_rwlock_t rwlock ;
};

////	2，用条件变量实现读写锁	用条件变量+互斥锁来实现。
//			注意：条件变量必须和互斥锁一起使用，等待、释放的时候都需要加锁
class Lock1_RW{
public:
	Lock1_RW(){
		_mtx = PTHREAD_MUTEX_INITIALIZER;
		_cnd = PTHREAD_COND_INITIALIZER;
		cnt_r = 0;
		cnt_w = 0;
	};

	void lock_r();
	void free_r();
	void lock_w();
	void free_w();
private:	
	pthread_mutex_t _mtx ;
	pthread_cond_t	_cnd ;
	int 			cnt_r ;
	int 			cnt_w ;
};

////	3，用互斥锁实现读写锁   2个互斥锁+1个整型变量
class Lock2_RW{
public:
	Lock2_RW(){
		mtx_r = PTHREAD_MUTEX_INITIALIZER;
		mtx_w = PTHREAD_MUTEX_INITIALIZER;   
		cnt_rd = 0;
	};
	void lock_r()
	{
		pthread_mutex_lock(&(mtx_r));           
		if(0 == cnt_rd)  
			pthread_mutex_lock(&(mtx_w));  
		cnt_rd++;  
		pthread_mutex_unlock(&(mtx_r)); 
	}
	void free_r()
	{
		pthread_mutex_lock(&(mtx_r));  
		cnt_rd--;  
		if(0 == cnt_rd)  
			pthread_mutex_unlock(&(mtx_w));  
		pthread_mutex_unlock(&(mtx_r));  
	}
	void lock_w()	{ pthread_mutex_lock(&(mtx_w)); }
	void free_w()	{ pthread_mutex_unlock(&(mtx_w)); }
	
private:
	pthread_mutex_t mtx_r ;		//控制读取计数
	pthread_mutex_t mtx_w ;		//控制写入权限
	int cnt_rd ;
};


////	4，用信号量来实现读写锁
#include <semaphore.h>

class Lock3_RW{
public:
	Lock3_RW(){
		sem_init(&r_sem, 0, 1);
		sem_init(&w_sem, 0, 1);
		cnt_rd = 0;
	};
private:
	void lock_r()
	{
		sem_wait(&(r_sem));
		if(0 == cnt_rd)
			sem_wait(&(w_sem));
		cnt_rd++;
		sem_post(&(r_sem));
	}
	void free_r()
	{
		sem_wait(&(r_sem));
		cnt_rd--;
		if(0 == cnt_rd)
			sem_post(&(w_sem));
		sem_post(&(r_sem));  
	}
	void lock_w()	{ sem_wait(&(w_sem)); }
	void free_w()	{ sem_post(&(w_sem)); }
	
private:
	sem_t r_sem;		//保证读取计数
	sem_t w_sem;		//控制写入独占
	int cnt_rd;
};


#endif

////windows 实现代码
//
//class LockRW
//{
//public:
//	LockRW() : m_cnt(0), m_evt(NULL), m_cs(NULL), m_sem(NULL)
//	{
//		// 提倡的做法在专门的初始化函数里创建和初始化这些变量
//		::InitializeCriticalSection(&m_cs);
//		m_evt = ::CreateEvent(NULL, TRUE, TRUE, NULL);	// Event必须是手动重置，否则存在死锁隐患，即等待Event前，先被激活了
//		m_sem = ::CreateSemaphore(NULL, 1, 1, NULL);
//	}
//	~LockRW(){
//		::CloseHandle(m_sem);
//		::CloseHandle(m_evt);
//		::DeleteCriticalSection(&m_cs);
//	}
//public:
//	void lock_r()
//	{
//		::WaitForSingleObject(m_evt, INFINITE);
//		
//		::EnterCriticalSection(&m_cs);
//		if(0 == m_cnt++)
//		    ::WaitForSingleObject(m_sem, INFINITE);
//		::LeaveCriticalSection(&m_cs);
//	}
//	void free_r()
//	{
//	    ::EnterCriticalSection(&m_cs);
//	    if(0 == --m_cnt)
//	        ::ReleaseSemaphore(m_sem, 1, NULL);
//	    ::LeaveCriticalSection(&m_cs);
//	}
//	void LockRW::lock_w()
//	{
//		::ResetEvent(m_evt);
//		::WaitForSingleObject(m_sem, INFINITE);
//	}
//	
//	void LockRW::free_w()
//	{
//		::ReleaseSemaphore(m_sem, 1, NULL);
//		::SetEvent(m_evt);
//	}
//private:
//	volatile DWORD      m_cnt;	//用于读计数
//	CRITICAL_SECTION    m_cs;	//保护读计数
//	HANDLE              m_evt;	//
//	HANDLE              m_sem;	//等待没有读的信号量
//};
