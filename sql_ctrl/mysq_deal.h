#ifndef _MYSQ_DEAL_H_
#define _MYSQ_DEAL_H_

#include "mysq_pool.h"

#ifdef __cplusplus
extern "C" {
#endif

//	ע�����ʱ  ��֤��ȷ�����ӵ������õ��Ŀ�
// gcc  mysql.c -o mysq  -I/usr/include/ -L/usr/lib64/mysql  -lmysqlclient   -lm -lrt -ldl  -lstdc++ -lpthread

//Σ��
int check_rymd_wx(char *card_id,char** rst);

//����
int check_rymd_zt(char *card_id,char** rst);

#ifdef __cplusplus
}
#endif

#endif
