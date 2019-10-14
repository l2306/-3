
#include "order_market.h"

bool OrderShelf_upMan :: addOrder2Shelf(Order* pToDo) {
	cout<<"upMan 新入 " << pToDo <<endl;
	switch (pToDo->eBuySell())
	{
	case eBUY:
		{
			asset_one* asset =  _market->_asset_store->getAsset(pToDo->_account_number);
			if( NULL == asset){
				cout << "\t无此帐号 \n";
			}else{
				double fund_have = asset->_money;
				double fund_need = pToDo->price() * pToDo->quantity();
				cout <<"\t 资金 有"<< fund_have << " 需" << fund_need ;
				if (fund_have < fund_need){
					cout<< " don,t have enough money!!\n"<<
						endl<<pToDo ;
				}else{
					asset->_money_can -= fund_need;
					_market -> addOrder2Shelf(pToDo);
					cout<< "\tenough money can deal \n";
					goto END_CanEntry;
				}
			}
		}break;
	case eSELL:
		{
			asset_one* asset =  _market->_asset_store->getAsset(pToDo->_account_number);
			if( NULL == asset){
				cout << "\t无此帐号 \n";
			}else{	
				int position_can = 0; //= asset->position;
				if(position_can<pToDo->quantity()){
					cout<< "\t持仓不足 仓"<<position_can <<" 单"<<pToDo->quantity()
						<<"  "<< pToDo <<endl ;
				}else{
					cout<< "\t \n";
					goto END_CanEntry;
				}
			}
		}break;
	case eCANCEL:
		{
			cout<< "\t无此报单 \n";
			goto END_CanEntry;
		}break;
	default:
		break;
	}
	END_CanNot:
		;
		//回告 无效;
	END_CanEntry:
		;
	//mapOrderMarket[pToDo->stock_number].add;
}
