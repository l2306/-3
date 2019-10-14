#ifndef __QUEUE__
#define __QUEUE__

#include "_lck_rw.h"
#include <deque>
using namespace std;

template <class T>
class z_Queue
{
	public:
		z_Queue(int uQueSize = 0);			//队列的大小, 为0不限制
		~z_Queue();
        int size();							//获取队列大小
        void clear();							//清空队列
        bool isEmpty();							//判断是否为空
		bool push(const T& item);				//塞入元素队列
		bool pop(T& item);						//对首弹出元素
		bool getFront(T& item);					//得到对首, 但不删除
		bool setQueueMaxSize(int maxSize);	//设置最大长度

	private:
		deque<T>    m_que;			//STL 双向列表的插入和删除 效率比vector高
		Lock_RW     m_lock;			//读写锁
		int         m_uSize;		//队列大小
};

template <class T>
z_Queue<T>::z_Queue(int uQueSize)
{
	m_uSize = uQueSize;
}

template <class T>
z_Queue<T>::~z_Queue()
{
	m_que.clear();
}

template <class T>
bool z_Queue<T>::push(const T &item)
{
	lock_w<Lock_RW>  lck(&m_lock);
	if(m_uSize && (m_que.size() == m_uSize))  
		return false;
	m_que.push_back(item);
	return true;
}

template <class T>
bool z_Queue<T>::pop(T &item)
{
	lock_w<Lock_RW>  lck(&m_lock);
	if(m_que.size() == 0)
		return false;
	item = m_que.front();
	m_que.pop_front();
	return true;
}

template <class T>
bool z_Queue<T>::getFront(T &item)
{
	lock_r<Lock_RW>  lck(&m_lock);
	if(m_que.size() == 0)
		return false;
	item = m_que.front();
	return true;
}

template <class T>
bool z_Queue<T>::isEmpty()
{
	lock_r<Lock_RW>  lck(&m_lock);
	bool bRet = m_que.empty();
	return bRet;
}

template <class T>
void z_Queue<T>::clear()
{
	lock_w<Lock_RW>  lck(&m_lock);
	m_que.clear();
}

template <class T>
int z_Queue<T>::size()
{
	lock_r<Lock_RW>  lck(&m_lock);
	int uSize = m_que.size();
	return uSize;
}

template <class T>
bool z_Queue<T>::setQueueMaxSize(int maxSize)
{
	lock_w<Lock_RW>  lck(&m_lock);
	if(maxSize <= m_uSize)
		return false;
	return true;
}
#endif
