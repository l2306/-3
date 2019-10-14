
先落盘 再回报；

账户中的资金，持仓 判断 买卖订单有无效
	预先扣款或扣仓，防止等单款仓再次下单 减少可用
撤单 	无需判断，只判断账号有无效，
	直接穿入后台对已提交订单撤销
	
这里的资金款仓款仓，可设置读写锁

最终 落仓落款

//入口
//----买
//线程池	集中处理
//1	有效	价？款？
//2	准备	禁用指定款项
//		防止撮合中资源抢占读取信息

//----卖
//线程池	集中处理
//1	有效	仓？价？
//2	准备	扣款

//----撤
//撤销		货？仓？



//出口		成交 或者 未成
//----买
//	落仓,			回款

//----卖
//	落款,清仓

//----撤
//撤销		货？仓？

class OrderDoor
{
	int addOrderList_ToDo(Order* order){
		std::unique_lock<std::mutex> lck(_mtx);
		_listOrder_ToDo.push_back(order);
		_cnd_not_empty.notify_all();
		return 0;
	}
	void runMarket (){
		for(;;)
		{
			std::unique_lock<std::mutex> lck(_mtx);
			while( 0 == _listOrder_ToDo.size())
				_cnd_not_empty.wait(lck);
			dealOrder( _listOrder_ToDo.front() );
			_listOrder_ToDo.pop_front();
		}
	}
	
	//入到 相应的账号		//查询大量账号信息  最好存hashmap
	int putinto(Order* pToDo) {
		cout<<"新单入场 " << pToDo <<endl;
		switch (pToDo->eBuySell())
		{
		case eBUY:
			fund_have;
			funf_need=pToDo->_price()* pToDo->_quantity_needdeal;
			if(fund_have<funf_need){
				款不足;
			}else{
				fund_have -= funf_need;
				goto END_CanEntry;
			}
			break;
		case eSELL:
			position_have[pToDo->stock_number];
			if(fund_have<pToDo->_quantity_needdeal){
				仓不足;
			}else{
				position_have[pToDo->stock_number] -= pToDo->_quantity_needdeal;
				goto END_CanEntry;
			}				
			break;
		case eCANCEL:
			goto END_CanEntry
			break;
		default:
			break;
		}
	END_CanNot:
		;
		//回告 无效;
	END_CanEntry:
		mapOrderMarket[pToDo->stock_number];
	}
	
	int door(Order* pToDo) {
		cout<<"新单入场 " << pToDo <<endl;
		switch (pToDo->eBuySell())
		{
		case eBUY:
			fund_have;
			funf_need=pToDo->_price()* pToDo->_quantity_needdeal;
			if(fund_have<funf_need){
				款不足;
			}else{
				fund_have -= funf_need;
				goto END_CanEntry;
			}
			break;
		case eSELL:
			position_have[pToDo->stock_number];
			if(fund_have<pToDo->_quantity_needdeal){
				仓不足;
			}else{
				position_have[pToDo->stock_number] -= pToDo->_quantity_needdeal;
				goto END_CanEntry;
			}				
			break;
		case eCANCEL:
			goto END_CanEntry
			break;
		default:
			break;
		}
	END_CanNot:
		;
		//回告 无效;
	END_CanEntry:
		mapOrderMarket[pToDo->stock_number];
	}
}
