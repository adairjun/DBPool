#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <thread>
#include <gtest/gtest.h>
#include "mysql_connection_pool.h"

using std::cout;
using std::endl;
using std::thread;

const char* handler(MysqlPool* p_mypool, const char* sql) 
{
  // 从数据库连接池当中取出一个可用的连接
  MysqlObj* conn = p_mypool->getConnection();
  
  if (!conn) {
  	cout << "getConnection NULL pointer" << endl;
  	exit(-1);
  }
  
  QueryResult queryResult; 
  conn->ExecuteSql(sql, queryResult);
  
  p_mypool->releaseConnection(conn);
  
  for (int i=0;i<queryResult.getRowCount();i++) {
  	for(int j=0;j<queryResult.getColumnCount();j++) {
  		cout << queryResult.getElement(i,j) << " ";
  	}
  	cout << endl;
  }
  return "hello";
}

TEST(handlerTest, Test1)
{
  MysqlPool mypool;

  EXPECT_STREQ("hello", handler(&mypool, "select * from student"));
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
