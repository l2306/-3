 
#ifndef __ORDER__
#define __ORDER__

#include <string>
#include <iostream>

using namespace std;

string getStrCurTime();
string sysTime_usec();

typedef enum enumBuySell{
	eBUY	= 1,
	eSELL	= 2,
	eCANCEL	= 3,
}enum_BuySell;

class Order
{
public:
	Order(enum_BuySell eBuySell, double price, int quantity,string usr)
		:_eBuySell(eBuySell), _price(price), _account_number(usr),
		_quantity_original(quantity),  _quantity_needdeal(quantity)
	{
		_time_stamp = sysTime_usec();
	}
	~Order() {
	}
	const string stock_number(){
		return  _stock_number;
	};
	const enum_BuySell eBuySell() {
		return  _eBuySell;
	}
	const double price() {
		return  _price;
	}
	const int quantity() {
		return  _quantity_original;
	};
	//交易操作		两单撮合
	friend int deal_do(Order* pToDo,Order* pWait,int done);
	friend ostream& operator<<(ostream &os, const Order* p);
	
//	friend OrderMarket& OrderMarket::operator<<(Order* order);
	string			_stock_number;		//门类
	string			_time_stamp;
	string			_account_number;
	int				_quantity_needdeal;
private:
	enum_BuySell	_eBuySell;
	double			_price;
	int				_quantity_original;
};

#endif
