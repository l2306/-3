
#include "mysq_pool.h"

POOL_SQL pool_sql;

int sql_pool_setInfo(const char *host, const char *user, const char *passwd, 
	const char *db, unsigned int port/*, const char *unix_socket, unsigned long client_flag*/)
{
		sprintf(pool_sql.host, "%s",host);
		sprintf(pool_sql.dbnm, "%s",db);
		sprintf(pool_sql.user, "%s",user);
		sprintf(pool_sql.pswd, "%s",passwd);
		pool_sql.port = port;
}

//用pool_sql信息 初始化node
//return 0: connect success, 1 connect error, -1: init error
static int create_connect(POOL_SQL *pool_sql, SQL_SOCK_NODE *node)
{
	int ret = 0;
	int opt = 1;
//	char log_str_buf[LOG_STR_BUF_LEN] = {0};

	do
	{
		if (NULL == mysql_init(&node->fd))
		{
			ret = -1;
		}
		else
		{
			if(pthread_mutex_init(&node->sql_lock, NULL))
			{
				
			}

			if(!(node->mysql_sock = mysql_real_connect(&node->fd, pool_sql->host, pool_sql->user, pool_sql->pswd, pool_sql->dbnm, pool_sql->port, NULL, 0)))
			{
				node->sql_state = DB_DISCONN;
				ret = 1;
			}
			else
			{
				node->used = 0;
				node->sql_state = DB_CONN;
				mysql_options(&node->fd, MYSQL_OPT_RECONNECT, &opt);
				opt = 3;			//3S
				mysql_options(&node->fd, MYSQL_OPT_CONNECT_TIMEOUT, &opt);
				ret = 0;
			}
		}

	}while(0);

	return ret;
}

int sql_pool_create(int connect_pool_number)
{
	int index = 0;
	MYSQL fd_temp;
	pool_sql.sql_pool = malloc(sizeof(SQL_SOCK_NODE*)*connect_pool_number);

	mysql_init(&fd_temp);

	for(index = 0; index < connect_pool_number; index ++)
	{
		SQL_SOCK_NODE* new_node = malloc(sizeof(SQL_SOCK_NODE));
		memset(new_node,0,sizeof(SQL_SOCK_NODE));
		if(-1 == create_connect(&pool_sql, new_node))
		{
			goto POOL_CREATE_FAILED;
		}
		pool_sql.sql_pool[index] = new_node;
		pool_sql.sql_pool[index]->index = index;
		pool_sql.pool_number++;
	}
	return 0;
POOL_CREATE_FAILED:

	sql_pool_destroy();
	return -1;
}

void sql_pool_destroy()
{
	printf("destroy\n");
	int index;
	for (index = 0; index < pool_sql.pool_number; index ++)
	{
		if (NULL != pool_sql.sql_pool[index]->mysql_sock) // close the mysql
		{
			mysql_close(pool_sql.sql_pool[index]->mysql_sock); // close
			free(pool_sql.sql_pool[index]);
			pool_sql.sql_pool[index] = NULL;
		}
	}
	free(pool_sql.sql_pool);
}


SQL_SOCK_NODE *get_db_connect_from_pool()
{
	int start_index = 0, loop_index = 0, index = 0, ping_ret = 0;

	srand((int) time(0));
	start_index = rand() % pool_sql.pool_number;

	for (loop_index = 0; loop_index < pool_sql.pool_number; loop_index++)
	{
		index = (start_index + loop_index) % pool_sql.pool_number;	//随机索引
		
		if(pool_sql.sql_pool[index])
		{
		if (0 == pthread_mutex_trylock(&(pool_sql.sql_pool[index]->sql_lock)))
		{
			if (DB_DISCONN == pool_sql.sql_pool[index]->sql_state)
			{
				//短链修复
				if (0 != create_connect(&pool_sql, pool_sql.sql_pool[index]))
        		{
					//释放锁
        			release_sock_to_sql_pool(pool_sql.sql_pool[index]);
        			continue;
        		}
			}
			ping_ret = mysql_ping(pool_sql.sql_pool[index]->mysql_sock);
			if (0 != ping_ret)
			{
				pool_sql.sql_pool[index]->sql_state = DB_DISCONN;
				release_sock_to_sql_pool(pool_sql.sql_pool[index]);
			}
			else
			{
				pool_sql.sql_pool[index]->used = 1;
				break;
			}
		}
		}
		

	}
	if (loop_index == pool_sql.pool_number)
	{
		return NULL;
	}
	else
	{
		return pool_sql.sql_pool[index];
	}
}

// put to pool
void release_sock_to_sql_pool(SQL_SOCK_NODE * n)
{
	n->used = 0;
	pthread_mutex_unlock(&n->sql_lock);
}

// error
void check_sql_sock_normal(SQL_SOCK_NODE * n)
{
	n->sql_state = DB_DISCONN;
}

