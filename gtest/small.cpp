#include "../include/pool.h" 
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <thread>

using std::cout;
using std::endl;
using std::thread;

void handler(Pool* p_mypool, const char* sql) 
{
	// 因为这里是用Connection对象来保存一个mysql连接,pool连接池就是一口气创建最大的连接数目,当需要mysql连接的时候就从pool当中取出来
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

	// mysql的最大连接数max_connections是151
	Pool mypool;
	
	// 设置要启动的线程数量
	const int THREAD_COUNT = 4;
	// 构造线程数组可以使用vector<thread>
	vector<thread> thread_array;
	for(long i=0;i<THREAD_COUNT;++i)
	{
		// 用一个线程启动handler,然后把这个线程放入线程数组当中去
		thread_array.push_back(thread(handler, &mypool, "select * from student"));
		//sleep(1);
	}

	for(int i=0;i<THREAD_COUNT;++i)
	{
		thread_array[i].join();
	}

	MysqlObj* conn = mypool.getConnection();
	QueryResult queryResult; 
	conn->ExecuteSql("INSERT INTO student VALUES(3,\"zhuying\");", queryResult);
	mypool.releaseConnection(conn);
	for(int i=0;i<queryResult.getRowCount();i++)
	{
		for(int j=0;j<queryResult.getColumnCount();j++)
		{
			cout << queryResult.getElement(i,j) << " ";
		}
		cout << endl;
	}
		
	return 0;
}
