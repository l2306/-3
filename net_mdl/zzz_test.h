#ifndef __USERWORKER__
#define __USERWORKER__


#include "define.h"

#include "zz_server.h"

using namespace std;


typedef struct typ_1{
	int usr;
	int pswd;
	char info[1024*8];
	short sChkSum;
}typ_1;

class UserWorker : public z_Worker
{
	public:
		UserWorker(unsigned int num) : z_Worker(num)
		{
		}

		~UserWorker()
		{
		}

		void rcvMsg(tlv_t *pTLV_i, int nSocket)
		{
			static int i = 0;
			static int i2 = 0;
			cout << " rcv_cnt total : " << ++i << " ";
			if(1==(int)(pTLV_i->tag))
			{
				cout <<"tag "<< (int)pTLV_i->tag << pTLV_i->len << pTLV_i->value << endl;
			}		
			if(2==(int)(pTLV_i->tag))
			{
				typ_1 t1;
				memcpy(&t1, pTLV_i->value, sizeof(typ_1));

				cout << " rcv_cnt " << ++i2 << endl;
				cout <<"\ttag2 "<< (int)pTLV_i->tag <<" len "<< pTLV_i->len \
						<<"\t <typ_1> "<< t1.usr <<" "<< t1.pswd <<" "<<t1.sChkSum<< endl;
			}
				
			free( pTLV_i );		//释放载体

			tlv_t* pTLV_o = NULL;
			sndMsg(nSocket, pTLV_o);
		}
};

class MainPro  : public z_Server
{
	public:
		MainPro()
		{
			userWork = new UserWorker(2);
			userWork->bind(9999);
		}

		~MainPro()
		{
			if(userWork)
			{
				delete userWork;
				userWork = NULL;
			}
		}
	
		static MainPro *getInstance()
		{
			if(!instance)
				instance = new MainPro();
			return instance;
		}

		void delInstance()
		{
			if(instance)
			{
				delete instance;
				instance = NULL;
			}
		}

		bool init()
		{
				z_Server::init();
		}
		virtual void final()
		{
			//回收资源
		}
	
	private:
		UserWorker *userWork;

		static MainPro *instance;
};

#endif
