
#ifndef _ORDER_MARKET_H_
#define _ORDER_MARKET_H_

#include "../__utils/_thread.h"
#include "../__utils/_hashmap.h"
#include "asset_store.h"
#include "order.h"
#include "order_shelf.h"

class Order;
class OrderShelf;
class AssetStore_asker;
class OrderShelf_upMan;
class OrderShelf_dnMan;
class OrderShelf_board;
class OrderMarket;

//查询	查询资产状况  用于报单
class AssetStore_asker : public z_Thread
{
public:
	AssetStore_asker(OrderMarket* market)
		: _market(market)
	{

	}
	~AssetStore_asker(){
	}
	//
	void Run(){
		for(;;)
		{		
		}
	}
private:
	list<Order*>				_listOrder_ToDo;
	OrderMarket* _market;
};

//上架	//判断仓款  限制仓库
class OrderShelf_upMan : public z_Thread
{
public:
	OrderShelf_upMan(OrderMarket* market)
		: _market(market)
	{
	}
	//填入到上架货架
	int add_todo(Order* order){
		std::unique_lock<std::mutex> lck(_mtx);
		_listOrder_ToDo.push_back(order);
		_cnd_not_empty.notify_all();
		return 0;
	}
	void do_work(){
		for(;;)
		{
			std::unique_lock<std::mutex> lck(_mtx);
			while( 0 == _listOrder_ToDo.size())
				_cnd_not_empty.wait(lck);

			addOrder2Shelf( _listOrder_ToDo.front() );		//检验订单 摆入货架
			_listOrder_ToDo.pop_front();
		}
	}

	//
	bool addOrder2Shelf(Order* pToDo);

	//
	void Run(){
		do_work();
	}

private:
	std::mutex					_mtx;				//保安
	std::condition_variable	_cnd_not_empty;		//非空
	list<Order*>				_listOrder_ToDo;
	OrderMarket* _market;
};

//下架	//1，修改资产	2，出单回告
class OrderShelf_dnMan : public z_Thread
{
public:
	OrderShelf_dnMan(OrderMarket* market)
		: _market(market){
	}
	//填入到下架货架
	int add_todo(Order* order){

	}
	//	
	void Run(){
	}
	
private:
	std::mutex					_mtx;				//保安
	std::condition_variable	_cnd_not_empty;		//非空
	list<Order*>				_listOrder_ToDo;
	OrderMarket* _market;
};

//看板	//公告 订单的货架信息   1，两订单处理  2，单个订单处理
class OrderShelf_board : public z_Thread
{	
public:
	OrderShelf_board(OrderMarket* market)
		: _market(market)
	{
	}
	int addOrder(Order* order){

	}
	//	
	void Run(){
		for(;;)
		{
			sleep(10000);
		}
	}
private:
	std::mutex					_mtx;				//保安
	std::condition_variable	_cnd_not_empty;		//非空
	list<Order*>				_listOrder_ToDo;
	OrderMarket* _market;
};

//
#include <string>
#include <iostream>
#include <string.h>
using namespace std;

class _HashFunc{
public:
	int operator()(const string & key){
		int hash = 0;				//此哈希取值函数 不是很好
		for (int i = 0; i < key.length(); ++i)
			hash = hash << 7 ^ key[i];
		return (hash & 0x7FFFFFFF);
	}
};
class _EqualKey{
public:
	bool operator()(const string & A, const string & B){
		if (A.compare(B) == 0)
			return true;
		else
			return false;
	}
};



//整个市场
class OrderMarket
{
public:
	OrderMarket(){
		init();
		start();
	}
	~OrderMarket(){
		delete asker;
		delete upMan;
		delete dnMan;
		delete board;
		//delete mapOrderShelf;
	}

	bool init(){
		asker = new AssetStore_asker(this);
		upMan = new OrderShelf_upMan(this);
		dnMan = new OrderShelf_dnMan(this);
		board = new OrderShelf_board(this);
		mapOrderShelf = new HashMap<string, OrderShelf*, _HashFunc, _EqualKey>(100,NULL);
		return true;
	}
	bool start(){
		asker->Start();
		upMan->Start();
		dnMan->Start();
		board->Start();
		return true;
	}	
	bool setAssetStore(AssetStore* p){
		_asset_store = p;
	}
	bool test_addOrder(Order* order){
		#define random(x) (rand()%x)
		for(int x=0; x<100; x++){
			int		price = random(49)+1;
			int		number = random(100);
			int		buysell= number%2+1;
			char	str[25]={0};
			sprintf(str, "%04d", x);
			upMan->add_todo(new Order((enum_BuySell)buysell, price, number, str));
		}
	}
	//不能随便暴露
	bool addOrder2Shelf(Order* order)
	{
		string str = order->stock_number();
		OrderShelf* shelf = mapOrderShelf->get(str);
		if( NULL== shelf )
			mapOrderShelf->add(str,new OrderShelf(str));
		shelf = mapOrderShelf->get(str);
		shelf->addOrderList_ToDo(order);
		return true;
	}
	friend AssetStore_asker;
	friend OrderShelf_upMan;
	friend OrderShelf_dnMan;//下架
	friend OrderShelf_board;//公告

private:
	AssetStore* 		_asset_store;
	AssetStore_asker*		asker;//询问
	OrderShelf_upMan*		upMan;//上架
	OrderShelf_dnMan*		dnMan;//下架
	OrderShelf_board*	 	board;//公告
	HashMap<string, OrderShelf*, _HashFunc, _EqualKey>*	mapOrderShelf;	//市场
};


#endif
