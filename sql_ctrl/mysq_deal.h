#ifndef _MYSQ_DEAL_H_
#define _MYSQ_DEAL_H_

#include "mysq_pool.h"

#ifdef __cplusplus
extern "C" {
#endif

//	注意编译时  保证正确的链接到所有用到的库
// gcc  mysql.c -o mysq  -I/usr/include/ -L/usr/lib64/mysql  -lmysqlclient   -lm -lrt -ldl  -lstdc++ -lpthread

//危险
int check_rymd_wx(char *card_id,char** rst);

//在逃
int check_rymd_zt(char *card_id,char** rst);

#ifdef __cplusplus
}
#endif

#endif
