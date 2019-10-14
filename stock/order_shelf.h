#ifndef _ORDER_SHELF_H_
#define _ORDER_SHELF_H_

#include <stddef.h> 
#include <stdio.h>  
#include <time.h>
#include <stdlib.h>

#include <list>
#include <string>
#include <iostream>

#include <mutex>
#include <thread>
#include <condition_variable>

#include "../__utils/_thread.h"
#include "order.h"

using namespace std;

#define TEST_ORDER_SHELF
#ifdef TEST_ORDER_SHELF
int test_order_shelf();
#endif

//包含  相同价格对应订单列表，
class OrderList_SamePrice
{
public:
	OrderList_SamePrice(double price, enum_BuySell e)
		:_price(price), _eBuySell(e),_quantity(0)
	{
		_listOrder.clear();
	}
	~OrderList_SamePrice(){
		list<Order*>::iterator itOrder = _listOrder.begin();
		while (itOrder != _listOrder.end())
				_listOrder.erase(itOrder++);
	}
	const enum_BuySell eBuySell() {
		return  _eBuySell;
	}
	const double price() {
		return  _price;
	}
	const int quantity() {
		return  _quantity;
	}
	int	quantityEncr(int num) {
		_quantity += num;
		return 0;
	};
	int	quantityDecr(int num) {
		_quantity -= num;
		return 0;
	};
	int addListOrder(Order* p) {
		_quantity += p->_quantity_needdeal;
		_listOrder.push_back(p);
		return 0;
	}
	list<Order*>* getListOrder() {
		return &_listOrder;
	}
private:
	enum_BuySell	_eBuySell;
	double			_price;
	int				_quantity;
	list<Order*>	_listOrder;
};

class OrderShelf : public z_Thread
{
public:
	OrderShelf(string stock_number)
		:_stock_number(stock_number)
	{
		_list_OrderList_buy.clear();
		_list_OrderList_sell.clear();
	}
	~OrderShelf() {
		list<OrderList_SamePrice*>::iterator it;
		it = _list_OrderList_buy.begin();
		while (it != _list_OrderList_buy.end())
			_list_OrderList_buy.erase(it++);

		it = _list_OrderList_sell.begin();
		while (it != _list_OrderList_sell.end())
			_list_OrderList_sell.erase(it++);
		//for(auto same: _list_OrderList_buy)
		//	delete same;
		//for(auto same: _list_OrderList_sell)
		//	delete same;
	}

	//处理订单	对撤单不处理
	int dealOrder(Order* pToDo); 

	//对于未处理的订单 加入等处理队列 
	int addOrderList(Order* p);

	//广播	可通过传确定邻近买卖队列中的  最有可能成交的 那个价位订单状况
	int	broadMarket();
	
	int addOrderList_ToDo(Order* order){
		std::unique_lock<std::mutex> lck(_mtx);
		_listOrder_ToDo.push_back(order);
		_cnd_not_empty.notify_all();
		return 0;
	}
	void runShelf (){
		for(;;)
		{
			std::unique_lock<std::mutex> lck(_mtx);
			while( 0 == _listOrder_ToDo.size())
				_cnd_not_empty.wait(lck);
			dealOrder( _listOrder_ToDo.front() );
			_listOrder_ToDo.pop_front();
		}
	}
		//	
	void Run(){
		runShelf ();
	}
	OrderShelf& operator << (Order* order);
	string stock_number(){
		return _stock_number;
	}
	
private:
	string					_stock_number;		//门类
	double					_price_open;		//开盘
	double					_price_up;			//上限		
	double					_price_dn;			//下限		
	std::mutex					_mtx;				//
	std::condition_variable			_cnd_not_empty;		//非空
	list<Order*>				_listOrder_ToDo;		//待处理订单
	list<OrderList_SamePrice*>		_list_OrderList_sell;
	list<OrderList_SamePrice*>		_list_OrderList_buy;
};

#endif
