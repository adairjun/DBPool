#include "../include/mysql_connection_pool.h"
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <thread>
#include <gtest/gtest.h>

using std::cout;
using std::endl;
using std::thread;

void handler(MysqlPool* p_mypool, const char* sql) 
{
	// 从数据库连接池当中取出一个可用的连接
	MysqlObj* conn = p_mypool->getConnection();

	if(!conn){
		cout << "getConnection NULL pointer" << endl;
		exit(-1);
	}
	
	QueryResult queryResult; 
	conn->ExecuteSql(sql, queryResult);

	p_mypool->releaseConnection(conn);

	for(int i=0;i<queryResult.getRowCount();i++)
	{
		for(int j=0;j<queryResult.getColumnCount();j++)
		{
			cout << queryResult.getElement(i,j) << " ";
		}
		cout << endl;
	}
}


int main(int argc,  char* argv[])
{

	MysqlPool mypool;
	
	// 设置要启动的线程数量
	const int THREAD_COUNT = 4;
	vector<thread> thread_array;
	for(long i=0;i<THREAD_COUNT;++i)
	{
		// 用一个线程启动handler,然后把这个线程放入线程数组当中去
		thread_array.push_back(thread(handler, &mypool, "select * from student"));
		//sleep(1);
	}

	// 每个线程启动join
	for(int i=0;i<THREAD_COUNT;++i)
	{
		thread_array[i].join();
	}

	return 0;
}
