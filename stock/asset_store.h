
#include <string.h>
#include <map>
#include <list>
#include <string>
#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "../__utils/_hashmap.h"
#include "../__utils/_lck_rw.h"
#include "../__utils/_thread.h"

using namespace std;

class asset_personal;

struct _HashFunc2{
	int operator()(const string & key){
		int hash = 0;				//此哈希取值函数 不是很好
		for (int i = 0; i < key.length(); ++i)
			hash = hash << 7 ^ key[i];
		return (hash & 0x7FFFFFFF);
	}
};
class _EqualKey2{
public:
	bool operator()(const string & A, const string & B){
		if (A.compare(B) == 0)
			return true;
		else
			return false;
	}
};


class position_t{
	string		_account_number;
	string		_stock_number;
	string		_time_stamp;
};

class asset_one
{
public:
	asset_one(string idx,double money)
		:_account_number(idx), _money(money),_money_can(money)
	{
	}
	~asset_one(){
	}
public:
	string 			_account_number;
	double			_money;
	double			_money_can;
	list<position_t*>		_map_position;
};

class AssetStore
{
public:
	AssetStore(){
		_lckRW = new Lock_RW();
		mapAssetStore = new HashMap<string, asset_one*, _HashFunc2, _EqualKey2>(1024,NULL);
		init();
	}
	~AssetStore(){
	}
	bool init(){
		for(int x=0; x<100; x++){
			char	str[25]={0};
			sprintf(str, "%04d", x); 
			mapAssetStore->add(str,new asset_one(str,10000));
		}
	}
	//最好不要这样返回	直接操作类实例 不太合适
	asset_one* getAsset(string str){
		return mapAssetStore->get(str);
	}
	double getAccountMoney_canuse(string strA){
		lock_r<Lock_RW> lck_r(_lckRW);
		mapAssetStore->get(strA);
		return 0;
	}	
	int setAccountMoney_canuse(string strA, double money){
		lock_r<Lock_RW> lck_w(_lckRW);
		mapAssetStore->get(strA);
		return 0;
	}
	int getAccountPosition_canuse(string strA, string str){
		mapAssetStore->get(strA);
		return 0;
	}	
	int setAccountPosition_canuse(string strA, double money){
		mapAssetStore->get(strA);
		return 0;
	}

private:
	Lock_RW* 	_lckRW;
	HashMap<string, asset_one*, _HashFunc2, _EqualKey2>* mapAssetStore;	//资产信息
};

