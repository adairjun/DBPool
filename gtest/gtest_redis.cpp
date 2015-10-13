#include "../include/redis_connection_pool.h"
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <thread>
#include <gtest/gtest.h>

using namespace std;

using std::cout;
using std::endl;
using std::thread;

const char* handler(RedisPool* p_redispool, const char* cmd) {
  RedisObjPtr conn = p_redispool->getConnection();
  if (!conn) {
    cout << "Redis pool getConnection NUll pointer" << endl;
    exit(-1);
  }

  string result;
  if (conn->StringResult(result)!=1) {
    cerr << "get StringResult error" << endl;
  }
  
  p_redispool->releaseConnection(conn);
  cout << "get result is " << result << endl;  
  return result.c_str();
}

TEST(handlerTest, Test1) {
  RedisPool redispool;
  EXPECT_STREQ("hello", handler(&redispool, "get student"));
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
