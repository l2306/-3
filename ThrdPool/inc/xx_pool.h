
#ifndef _XXX_POOL_H
#define _XXX_POOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include"debug.h" 
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include"xx_list.h"		//通用链表

typedef void* (*pfun)(void *);

//-------------任务池的设计-------------

//控制线程间同步的数据结构		//已经废弃不用
typedef struct _thcontrol{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
}thcontrol;  

//任务链表的节点
typedef struct taskList{
	void *task;						//任务节点内容，由开发人员自己定义数据结构
	struct lst_head head;			//通用链表的指针
}taskList; 

/* typedef struct taskNode{
	void *task;						//任务节点内容，由开发人员自己定义数据结构
	struct taskNode *prev,*next;	//无封装使用复杂
}taskNode; */

//任务池
typedef struct TaskPool{
	unsigned int t_maxNum;			//允许的最大的任务数
	unsigned int t_curNum;			//当前任务数量
	void* (*func)(void*);			//个任务具体要做的操作，函数指针实现接口
	taskList *pool;					//头指针 (上图中的head节点)
	pthread_mutex_t mutex;			//保证数据安全
	pthread_cond_t cond_nofull;		//任务满 加任务阻塞 线程完工发信号
	pthread_cond_t cond_noempty;	//任务空 死循环投递入线程阻塞  添加任务发信号
}TaskPool;

extern TaskPool *TaskPoolInit();			//池初始化
extern void *TaskPoolAdd(TaskPool* taskpool, void *node);	//添加任务
extern void *TaskPoolDel(TaskPool* taskpool);				//删除任务(应该有规则删除什么样的)	//del from list ...(queue)

//-------------线程池的设计-------------

//线程节点数据结构
typedef struct threadNode{
		void* pthreadpool; 	//用时需要强转
		pthread_t tid;		//该线程的ID
		int busy;			//该线程是否空闲
		void *task;			//该线程处理的任务接点，这里指向一个任务节点
		thcontrol control;	//控制结构  xxxxxxx
}threadNode;

//线程链表结构
typedef struct threadList{
        threadNode *thread;
        struct lst_head head;
}threadList;

//线程池的数据结构
typedef struct ThreadPool{
	unsigned int t_maxNum;		//池允许最大线程
	unsigned int t_maxIdleNum;	//空闲线程上限
	unsigned int t_minIdleNum;	//空闲线程下限
	unsigned int t_idleNum;		//当前空闲线程
	unsigned int t_curNum;		//当前工作线程
	void* (*fun)(void *);  		//该线程执行的任务，这个函数指针指向任务节点的接口函数
	threadList *pool;			//双向链表的头
	pthread_mutex_t mutex;
	pthread_cond_t cond_idle;
}ThreadPool;

extern ThreadPool *ThreadPoolInit();			//初始化
extern int setThreadPool(ThreadPool* threadpool, int maxNum, int maxIdleNum, int minIdleNum);//设置线程属性
extern int createThreadPool();					//创建池	//不用
extern int increaseThreadPool(ThreadPool* threadpool);				//增加线程 +1
extern int decreaseThreadPool(ThreadPool* threadpool);				//减少线程 -1
extern int addTaskToThreadPool(ThreadPool* threadpool, void *task);	//从任务池中获得一个节点并投递给线程池

extern void threadWork(void *node);		//

//------------
typedef struct{
	TaskPool* taskpool;
	ThreadPool* threadpool;
	pthread_t mng_inputTsk;
	pthread_t mng_task2work;
	pthread_t mng_thrNumCtrl;
}ThrdPool;

/*	池信息初始化
返回值：
参数：任务池容量 最大线程个数 线程最大闲置 线程最小闲置 线程体函数
*/
ThrdPool* ThrdPool_init(int taskpoolnum, int maxNum, int maxIdleNum, int minIdleNum, void* (*func)(void*));

/*	开启任务投递线程
返回值：任务入pool投递线程id
参数：任务点投入taskpool的函数指针   注：此函数必须使用ThrdPool_addTaskToTaskpool进行投递
*/
pthread_t managerInterface(ThrdPool* thrdpool, void* (*inputTsk)(void*));

/*
返回值：0，添加正常	1，终止添加 2，已满
参数：任务入口参数
*/
int ThrdPool_addTask(ThrdPool* thrdpool, void *task);

/*	程开工任务	死循环 任务到执行线程
*/
pthread_t ThrdPool_run(ThrdPool* thrdpool);
	static void managerMoveTaskToWorker(void* thrdpool);
	void manager_moveTaskToWorker(ThrdPool* thrdpool);

/*	线程数量管理	循环看数量 控制合理的数量
*/
pthread_t ThrdPool_NumCtl(ThrdPool* thrdpool);
	static void* managerThreadNum(void* thrdpool);

/*	完工销毁接口			启动线程
*/

void ThrdPool_destroy(ThrdPool* thrdpool);

#ifdef __cplusplus
}
#endif

#endif
