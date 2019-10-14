

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "mysq_pool.h"
#include "mysq_deal.h"

int check_rymd_wx(char *card_id,char** rst)
{
	int 	ret = 0;
	MYSQL 	mysql;
	MYSQL	*connect = NULL;
	
	connect = mysql_init(&mysql);
	if (connect == NULL)
	{
		ret =  mysql_errno(&mysql) ;
		printf("func mysql_init() err \n");
		return ret;
	}
	printf("func mysql_init() ok \n");
	
	
	connect = mysql_real_connect(connect, "localhost", "dbuser", "1234567", "mydb", 0, NULL, 0);
	if (connect == NULL)
	{
		ret =  mysql_errno(&mysql) ;
		printf("func mysql_init() err \n");
		return ret;	
	}
	printf("func mysql_real_connect() ok \n");
	
	//查询
	const char query[256]={0};
//	const char *query = "select * from user where host=\'kingdom\'";
	sprintf(query,"select * from rymd_zs where card_id=\'%s\'",card_id);
	
	ret = mysql_query(connect, query);
	if (ret != 0)
	{
		ret =  mysql_errno(connect) ;
		printf("func mysql_query() err \n");
		return ret;	
	}
	
	//typedef char **MYSQL_ROW;               /* return data as array of strings */
	//typedef unsigned int MYSQL_FIELD_OFFSET; /* offset to current field */

	//获取结果集合 
	//结果集合中 可以含有10行数据
	MYSQL_RES *result = mysql_store_result(connect);
	
	MYSQL_ROW row = NULL;
	while (row = mysql_fetch_row(result) ) 
	{
		printf("%s, %s, %s\n", row[0], row[1],row[2]);
		*rst=malloc(strlen(row[1])+1);
		memset(*rst,0,strlen(row[1])+1);
		strcpy(*rst,row[1]);
			
//rintf("%s, %s, %s, %s, %s, %s, %s, %s \n", row[0], row[1],row[2],row[3],row[4],row[5],row[6],row[7]);
	}
	
	mysql_free_result(result);

	mysql_close(&mysql);
	
	printf("hello...\n");
	return ret;
}


int check_rymd_zt(char *card_id,char** rst)
{
	
}
