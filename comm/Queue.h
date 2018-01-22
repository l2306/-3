#ifndef __QUEUE__
#define __QUEUE__
// 
//	队列的简单封装 , 队列操作时加了锁的保护

#include "Lock.h"
#include "def.h"

template <class T>
class z_Queue
{
	public:
		//构造函数 
		//m_uQueSize 限制队列的大小, 如果m_uQueSize为0不限制队列的大小
		z_Queue(ULONG uQueSize = 0);

		// 析构函数
		~z_Queue();

		//压一个元素入队列
		//item 一个元素
		// 如果为false 表示失败,如果为true表示成功
		bool push(const T &item);

		//从对首弹出一个元素
		//item 存放在item
		// 如果为false 表示失败 ,如果为true表示成功
		bool pop(T &item);

		//得到对首一个元素 , 但是不删除对首
		//item 存放在item
		// 如果为false 表示失败 , 如果为true表示成功
		bool getFront(T &item);

		//判断队列是否为空
		// true为空队列 , false不为空队列
		bool isEmpty();

		//清空队列
		void clear();

		//去队列的大小
		// 队列的大小
		ULONG size();

		//设置队列最大长度
		//maxSize 最大值
		// 如果maxSize 比m_uQueSize 小返回false , 否则返回true
		bool setQueueMaxSize(ULONG maxSize);

	private:
		
		deque<T> m_que;		//STL 提供双向列表在插入和删除的操作中效率比vector高

		z_Lock m_lock;			//读写锁

		ULONG m_uQueSize;		//队列大小
};

template <class T>
z_Queue<T>::z_Queue(ULONG uQueSize)
{
	m_uQueSize = uQueSize;
}

template <class T>
z_Queue<T>::~z_Queue()
{
	m_que.clear();
}

template <class T>
bool z_Queue<T>::push(const T &item)
{
	m_lock.wLock();
	if(m_uQueSize && (m_que.size() == m_uQueSize))  //如果限制队列大小才进行比较
	{
		m_lock.unwLock();
		return false;
	}
	
	m_que.push_back(item);
	m_lock.unwLock();
	return true;
}

template <class T>
bool z_Queue<T>::pop(T &item)
{
	m_lock.wLock();
	if(m_que.size() == 0)
	{
		m_lock.unwLock();
		return false;
	}

	item = m_que.front();
	m_que.pop_front();
	m_lock.unwLock();
	return true;
}

template <class T>
bool z_Queue<T>::getFront(T &item)
{
	m_lock.rLock();
	if(m_que.size() == 0)
	{
		m_lock.unwLock();
		return false;
	}
	item = m_que.front();
	m_lock.unrLock();
	return true;
}

template <class T>
bool z_Queue<T>::isEmpty()
{
	m_lock.rLock();
	bool bRet = m_que.empty();
	m_lock.unrLock();
	return bRet;
}

template <class T>
void z_Queue<T>::clear()
{
	m_lock.wLock();
	m_que.clear();
	m_lock.unwLock();
}

template <class T>
ULONG z_Queue<T>::size()
{
	m_lock.rLock();
	ULONG uSize = m_que.size();
	m_lock.unrLock();
	return uSize;
}

template <class T>
bool z_Queue<T>::setQueueMaxSize(ULONG maxSize)
{
	m_lock.wLock();
	if(maxSize <= m_uQueSize)
	{
		m_lock.unwLock();
		return false;
	}

	m_lock.unwLock();
	return true;
}
#endif
