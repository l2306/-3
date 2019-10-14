
#include "order_shelf.h"

	/*
	处理提交的订单
		单价与队价	> 以队列价成交  
			订单对订单 撮合
			1,主买<被卖	买成
					报：	买全 卖半	
					跳出处理	goto
			2,主买=被卖	买成
					报：	买全 卖全	
					卖队减单	goto
			3,主买>被卖	再买
					报：	买办 卖全	
					卖队减单	cotinue
	*/
int OrderShelf::dealOrder(Order* pToDo) {
	cout<<"处理" << pToDo <<endl;
	switch (pToDo->eBuySell())
	{
	case eBUY:
		if (0 == _list_OrderList_sell.size()) 
			goto  End_allCanDealDone;
		else {
			cout << "  买单 " << pToDo->_quantity_needdeal << "  遍历 等卖队列 \n";
			list<OrderList_SamePrice*>::iterator itSame = _list_OrderList_sell.begin();
			while (itSame != _list_OrderList_sell.end())
			{
				if (pToDo->price() < (*itSame)->price())
					goto  End_allCanDealDone;
				else
				{
					cout << "   价格：" << (*itSame)->price() << endl;
					list<Order*>::iterator itOrder = (*itSame)->getListOrder()->begin();
					while (itOrder != (*itSame)->getListOrder()->end())
					{
						if (pToDo->_quantity_needdeal < (*itOrder)->_quantity_needdeal) {
							int done = pToDo->_quantity_needdeal;
							deal_do(pToDo, (*itOrder), done);
							(*itSame)->quantityDecr(done);
							goto End_allCanDealDone;
						}
						else if (pToDo->_quantity_needdeal == (*itOrder)->_quantity_needdeal) {
							int done = pToDo->_quantity_needdeal;
							deal_do(pToDo, (*itOrder), done);
							(*itSame)->quantityDecr(done);
							(*itSame)->getListOrder()->erase(itOrder++);
							goto End_allCanDealDone;
						}
						else if (pToDo->_quantity_needdeal > (*itOrder)->_quantity_needdeal) {
							int done = (*itOrder)->_quantity_needdeal;
							deal_do(pToDo, (*itOrder), done);
							(*itSame)->quantityDecr(done);
							(*itSame)->getListOrder()->erase(itOrder++);
							continue;
						}
						itOrder++;
					}
					if( 0==(*itSame)->getListOrder()->size() ){
						_list_OrderList_sell.erase(itSame++);
						continue ;
					}
				}
				itSame++;
			}
		}
		break;
	case eSELL:
		if (0 == _list_OrderList_buy.size()) 
			goto  End_allCanDealDone;
		else {
			cout <<"  卖单 "<<pToDo->_quantity_needdeal<< "  遍历 等买队列 \n";
			list<OrderList_SamePrice*>::iterator itSame = _list_OrderList_buy.begin();
			while (itSame != _list_OrderList_buy.end())
			{
				if (pToDo->price() > (*itSame)->price())
					goto  End_allCanDealDone;
				else
				{
					cout << "   价格：" << (*itSame)->price() << endl;
					list<Order*>::iterator itOrder = (*itSame)->getListOrder()->begin();
					while (itOrder != (*itSame)->getListOrder()->end())
					{
						if (pToDo->_quantity_needdeal < (*itOrder)->_quantity_needdeal) {
							int done = pToDo->_quantity_needdeal;
							deal_do(pToDo, (*itOrder), done);
							(*itSame)->quantityDecr(done);
							goto End_allCanDealDone;
						}
						else if (pToDo->_quantity_needdeal == (*itOrder)->_quantity_needdeal) {
							int done = pToDo->_quantity_needdeal;
							deal_do(pToDo, (*itOrder), done);
							(*itSame)->quantityDecr(done);
							(*itSame)->getListOrder()->erase(itOrder++);
							goto End_allCanDealDone;
						}
						else if (pToDo->_quantity_needdeal > (*itOrder)->_quantity_needdeal) {
							int done = (*itOrder)->_quantity_needdeal;
							deal_do(pToDo, (*itOrder), done);
							(*itSame)->quantityDecr(done);
							(*itSame)->getListOrder()->erase(itOrder++);
							continue;
						}
						itOrder++;
					}
					if( 0==(*itSame)->getListOrder()->size() ){
						_list_OrderList_buy.erase(itSame++);
						continue ;
					}
				}
				itSame++;
			}
		}
		break;
	default:
		break;
	}
	End_allCanDealDone:
		cout << "  剩余成交"<<pToDo->_quantity_needdeal <<"\n";
	if (pToDo->_quantity_needdeal > 0)
			addOrderList(pToDo);
	else
		delete pToDo;				//此单成交	释放订单(释放占用资金或者占用持仓)
	broadMarket();
	return 0;
}
	
//暂不能交易 插入待交易队列
//例如	买方队列是否为空 
		//	1，为空		创建改价格的队列
		//	2，非空		找到相同价格的队列 并放入
		//		直到找到 小于价格队列未曾找到  创建改价格的队列 插入到小鱼价格之前 
int OrderShelf::addOrderList(Order* p) {
	switch (p->eBuySell())
	{
	case eBUY:
		if (0 == _list_OrderList_buy.size()) {
			OrderList_SamePrice*  orderList_SamePrice
				= new OrderList_SamePrice(p->price(), p->eBuySell());
			orderList_SamePrice->addListOrder(p);
			_list_OrderList_buy.push_back(orderList_SamePrice);
		}else{
			list<OrderList_SamePrice*>::iterator itSame = _list_OrderList_buy.begin();
			while (itSame != _list_OrderList_buy.end())
			{
				if (p->price() < (*itSame)->price())
					;
				if (p->price() == (*itSame)->price())
				{
					(*itSame)->addListOrder(p);
					goto	FindLocation;
				}
				if (p->price() > (*itSame)->price())
				{
					OrderList_SamePrice*  orderList_SamePrice = new OrderList_SamePrice(p->price(), p->eBuySell());
					orderList_SamePrice->addListOrder(p);
					_list_OrderList_buy.insert(itSame, orderList_SamePrice);
					goto	FindLocation;
				}
				itSame++;
			}
			OrderList_SamePrice*  orderList_SamePrice
				= new OrderList_SamePrice(p->price(), p->eBuySell());
			orderList_SamePrice->addListOrder(p);
			_list_OrderList_buy.push_back(orderList_SamePrice);
		}
		break;
	case eSELL:
		if (0 == _list_OrderList_sell.size()) {
			OrderList_SamePrice*  orderList_SamePrice
				= new OrderList_SamePrice(p->price(), p->eBuySell());
			orderList_SamePrice->addListOrder(p);
			_list_OrderList_sell.push_back(orderList_SamePrice);
		}
		else {
			list<OrderList_SamePrice*>::iterator itSame = _list_OrderList_sell.begin();
			while (itSame != _list_OrderList_sell.end())
			{
				if (p->price() > (*itSame)->price())
					;
				if (p->price() == (*itSame)->price())
				{
					(*itSame)->addListOrder(p);
					goto	FindLocation;
				}
				if (p->price() < (*itSame)->price())
				{
					OrderList_SamePrice*  orderList_SamePrice = new OrderList_SamePrice(p->price(), p->eBuySell());
					orderList_SamePrice->addListOrder(p);
					_list_OrderList_sell.insert(itSame, orderList_SamePrice);
					goto	FindLocation;
				}
				itSame++;
			}
			OrderList_SamePrice*  orderList_SamePrice
				= new OrderList_SamePrice(p->price(), p->eBuySell());
			orderList_SamePrice->addListOrder(p);
			_list_OrderList_sell.push_back(orderList_SamePrice);
		}
		break;
	case eCANCEL:
		{
			list<OrderList_SamePrice*>::iterator itSame;
			itSame = _list_OrderList_buy.begin();
			while (itSame != _list_OrderList_buy.end())
			{
				if (p->price() < (*itSame)->price())
					;
				if (p->price() > (*itSame)->price())
					break;
				if (p->price() == (*itSame)->price())
				{
					list<Order*>::iterator itOrder = (*itSame)->getListOrder()->begin();
					while (itOrder != (*itSame)->getListOrder()->end())
					{
						//判断 就是该订单
						if( (*itOrder)->_time_stamp==p->_time_stamp && (*itOrder)->_account_number==p->_account_number)
						{
							//itOrder _time_stamp;//加入撤单序列 释放占用
							(*itSame)->getListOrder()->erase(itOrder++);
							goto END_CANCEL;
						}	
						itOrder++;
					}
					goto END_CANCEL;
				}
				itSame++;
			}
			itSame = _list_OrderList_sell.begin();
			while (itSame != _list_OrderList_sell.end())
			{
				if (p->price() > (*itSame)->price())
					;
				if (p->price() < (*itSame)->price())
					break;
				if (p->price() == (*itSame)->price())
				{
					list<Order*>::iterator itOrder = (*itSame)->getListOrder()->begin();
					while (itOrder != (*itSame)->getListOrder()->end())
					{
						//判断 就是该订单
						if( (*itOrder)->_time_stamp==p->_time_stamp && (*itOrder)->_account_number==p->_account_number)
						{
							//itOrder _time_stamp;	//加入撤单序列 释放占用
							(*itSame)->getListOrder()->erase(itOrder++);
							goto END_CANCEL;
						}	
						itOrder++;
					}
					goto END_CANCEL;
				}
				itSame++;
			}
	END_CANCEL:
		cout <<"成功取消订单";
		}
		break;
	default:
		break;
	}
	FindLocation:
		cout << "\n";					//找到位置
	return 0;
}
	
	
int OrderShelf::broadMarket(){
	list<OrderList_SamePrice*>::reverse_iterator itSame;
	cout << "" <<"行情 卖买"<<endl;
	itSame = _list_OrderList_sell.rbegin();
	while (itSame != _list_OrderList_sell.rend())
	{
		if( 0==(*itSame)->quantity() ){
			itSame++ ;				
			continue ;
		}else
			cout <<"\t"<< (*itSame)->price() <<"\t"<< (*itSame)->quantity()<<endl;
		itSame++ ;
	}
	{
	list<OrderList_SamePrice*>::iterator itSame;
	cout << "\t" <<"----"<<endl;
	itSame = _list_OrderList_buy.begin();
	while (itSame != _list_OrderList_buy.end())
	{
		if( 0==(*itSame)->quantity() ){
			itSame++ ;				
			continue ;
		}else
			cout <<"\t"<< (*itSame)->price() <<"\t"<<(*itSame)->quantity()<<endl;
		itSame++ ;	
	}
	}
}

OrderShelf& OrderShelf::operator<<(Order* order)
{
	std::unique_lock<std::mutex> lck(_mtx);
	_listOrder_ToDo.push_back(order);
	_cnd_not_empty.notify_all();
    return *this;
}



#ifdef TEST_ORDER_SHELF
#define random(x) (rand()%x)
int test_order_shelf() {
	OrderShelf shelf("某交易对象的货架");
	shelf.Start();

	shelf.addOrderList_ToDo(new Order(eSELL, 22, 250,"0000"));
  	shelf<<(new Order(eSELL, 22, 600,"2222"))<<(new Order(eSELL, 21, 200,"1111"));

	srand((int)time(0));
	for(int x=0; x<100; x++){
		int		price = random(49)+1;
		int		number = random(100);
		int		buysell= number%2+1;
		char	str[25]={0};
		sprintf(str, "%04d", x); 
		//itoa(number,str,10);
		shelf.addOrderList_ToDo(new Order((enum_BuySell)buysell, price, number, str));
	}

	cin.get();
	return 0;
}
#endif
