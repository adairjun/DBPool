#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <thread>
#include <gtest/gtest.h>
#include "mysql_connection_pool.h"

using std::cout;
using std::endl;
using std::thread;

const char* handler(MysqlPoolPtr p_mypool, const char* sql) {
  // 从数据库连接池当中取出一个可用的连接
  MysqlObjPtr conn = p_mypool->GetConnection();
  if (!conn) {
  	cout << "getConnection NULL pointer" << endl;
  	exit(-1);
  }
  // 保存sql执行结果
  QueryResult queryResult; 
  conn->ExecuteSql(sql, queryResult);
  // 释放连接
  p_mypool->ReleaseConnection(conn);
  // 循环输出sql执行结果 
  for (int i=0;i<queryResult.getRowCount();i++) {
  	for(int j=0;j<queryResult.getColumnCount();j++) {
  		cout << queryResult.getElement(i,j) << " ";
  	}
  	cout << endl;
  }
  return "hello";
}

TEST(handlerTest, Test1) {
  MysqlPoolPtr mysqlpool_ptr(new MysqlPool);
  EXPECT_STREQ("hello", handler(mysqlpool_ptr, "select * from student"));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
