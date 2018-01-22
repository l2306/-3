
#include "zz_worker.h"

z_Worker::z_Worker(unsigned int num)
{
	g_threadPool = new z_ThreadPool(num , this);
}

z_Worker::~z_Worker()
{
	if(g_threadPool)
	{
		delete g_threadPool;
		g_threadPool = NULL;
	}
}

bool z_Worker::Bind(unsigned int nPort)
{
	return g_threadPool->Bind(nPort);
}

int z_Worker::sndMsg(int nSocket, tlv_t *pTLV)
{
	if(!pTLV)
		return -1;
	return g_threadPool->sndMsg(nSocket, pTLV);
}
