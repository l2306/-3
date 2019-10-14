#ifndef _MYSQ_POOL_H_
#define _MYSQ_POOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <mysql/mysql.h>

//	注意编译时  保证正确的链接到所有用到的库
//gcc mysql_pool.c -o mysq -I/usr/include/ -L/usr/lib64/mysql -lmysqlclient -lm -lrt -ldl -lstdc++ -lpthread

#define DB_LEN_IP           15
#define DB_LEN_PORT         8
#define DB_LEN_DBNAME       64
#define DB_LEN_DBUSER       64
#define DB_LEN_PASSWD       64

typedef struct _SQL_SOCK_NODE{
	MYSQL					fd;
	MYSQL 		*		mysql_sock;	//单条
	pthread_mutex_t 			sql_lock;
	char					used;
	int 					index; 	// for test
	enum {DB_DISCONN, DB_CONN} 	sql_state;
} SQL_SOCK_NODE;			//

typedef struct _POOL_SQL{
	SQL_SOCK_NODE** sql_pool;

	char host[DB_LEN_IP + 1];
	char dbnm[DB_LEN_DBNAME + 1];
	char user[DB_LEN_DBUSER + 1];
	char pswd[DB_LEN_PASSWD + 1];
	int  port;
	int pool_number;
} POOL_SQL;

extern POOL_SQL pool_sql;


//设置数据库信息
int sql_pool_setInfo(const char *host, const char *user, const char *passwd, 
	const char *db, unsigned int port/*, const char *unix_socket, unsigned long client_flag*/);


//用pool_sql信息 初始化node
//return 0: connect success, 1 connect error, -1: init error
static int create_connect(POOL_SQL *pool_sql, SQL_SOCK_NODE *node);

//创建 number个数据库连接
int sql_pool_create(int connect_pool_number);

//销毁数据库连接池
void sql_pool_destroy();

//从池中 拿出一条连接
SQL_SOCK_NODE *get_db_connect_from_pool();

//将连接 放回连接池
void release_sock_to_sql_pool(SQL_SOCK_NODE * n);

//查看某连接的状态
void check_sql_sock_normal(SQL_SOCK_NODE * n);

#ifdef __cplusplus
}
#endif

#endif

