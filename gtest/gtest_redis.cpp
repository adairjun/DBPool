#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <thread>
#include <gtest/gtest.h>
#include "redis_connection_pool.h"

using std::cout;
using std::cerr;
using std::endl;
using std::thread;

const char* handler(RedisPoolPtr p_redispool, const char* cmd) { 
  // 取得redis连接
  RedisObjPtr conn = p_redispool->GetConnection();
  if (!conn) {
    cout << "Redis pool getConnection NUll pointer" << endl;
    exit(-1);
  }
  // 执行redis命令
  if (conn->ExecuteCmd(cmd)==-1) {
    cerr << "ExecuteCmd error" << endl;
  }
  // 确定了得到的值是string类型的,所以把redis命令的执行结果保存到result当中
  string result;
  if (conn->StringResult(result)!=1) {
    cerr << "get StringResult error" << endl;
  }
  // 释放连接 
  p_redispool->ReleaseConnection(conn);
  cout << "get result is " << result << endl;  
  return result.c_str();
}

TEST(handlerTest, Test1) {
  RedisPoolPtr redispool_ptr(new RedisPool);
  EXPECT_STREQ("hello", handler(redispool_ptr, "get student"));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
