#include "zzz_test.h"
#include "ThreadPool.h"

MainPro *MainPro::instance = NULL;

int fd()
{
	struct sockaddr_in serveraddr;
	int confd,len;
	char strip[16]="127.0.0.1";	
	char buf[1024];

	confd=socket(AF_INET,SOCK_STREAM,0);
	
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family	=AF_INET;
	inet_pton(AF_INET,strip,&serveraddr.sin_addr.s_addr);
	serveraddr.sin_port	=htons( 9999 );
	
	connect(confd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	return confd;
}

class sndTst : public z_Thread
{
	public:
		sndTst(z_threadSndMsg *SendMessThread);
		~sndTst();
		void Run();
		int sock;
		z_threadSndMsg* zz;
};
sndTst::sndTst(z_threadSndMsg * arg) : zz(arg)
{
	sock= fd();
	setSockBuffSize(sock , 8*8192 , 8*8192);
	zz->addEpoll(sock);
}
void sndTst::Run()
{
	//static 
	int cnt = 0;
	while(1)
	{
		if(++cnt>1000)
			sleep(60*60);
		else
			usleep(5000);

		typ_1 t1 ={11, sock, "", cnt};
		sprintf( t1.info ," data : %d  num : %d" , sock , cnt);

		tlv_t* pTLV2 =(tlv_t*)malloc(3 + sizeof(typ_1) );
			pTLV2->tag = 2;
			pTLV2->len = sizeof(typ_1);
		memcpy(pTLV2->value,&t1,sizeof(typ_1));
		zz->sndMsg(sock ,pTLV2);
		
		cout<<" sock " <<  sock <<" snd_cnt "<< cnt <<endl;
	}
}

int main(int argc,char* argv[])
{
#if 0
	MainPro *mp = MainPro::getInstance();
	mp->init();
		
#else
	int num_thr=5;
	int num_tst=8;
	for(int i=0;i<num_thr;i++)
	{
		z_threadSndMsg* zz = new z_threadSndMsg();
		for(int j=0;j<num_tst;j++)
		{
			usleep(1000000/num_thr/num_tst);
			sndTst* ss = new sndTst(zz);
			ss->start();
		}
		zz->start();
	}
#endif

	sleep(-1);
}
