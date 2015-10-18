#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <thread>
#include <Theron/Theron.h>
#include <gtest/gtest.h>
#include "DBPool/redis_connection_pool.h"

using std::cout;
using std::cerr;
using std::endl;

static RedisPoolPtr redispool_ptr(new RedisPool);

class Actor : public Theron::Actor {
 public:
  Actor(Theron::Framework &framework) : Theron::Actor(framework) {
    RegisterHandler(this, &Actor::Handler);
  }

 private:
  void Handler(const string &cmd, const Theron::Address from) {
    // 取得redis连接
    RedisObjPtr conn = redispool_ptr->GetConnection();
    if (!conn) {
      cout << "Redis pool getConnection NUll pointer" << endl;
      exit(-1);
    }
    // 执行redis命令
    if (conn->ExecuteCmd(cmd.c_str())==-1) {
      cerr << "ExecuteCmd error" << endl;
    }
    // 确定了得到的值是string类型的,所以把redis命令的执行结果保存到result当中
    string result;
    if (conn->StringResult(result)!=1) {
      cerr << "get StringResult error" << endl;
    }
    // 释放连接 
    redispool_ptr->ReleaseConnection(conn);
    cout << "get result is " << result << endl;  
	Send(0, from);
  }
};

const char* handler(const string &cmd) {
  Theron::Receiver receiver;
  Theron::Framework(framework);
  Actor actor(framework);
  framework.Send(cmd, receiver.GetAddress(), actor.GetAddress());
  receiver.Wait();
  return "hello"; 
}

TEST(handlerTest, Test1) {
  string cmd = "get student";
  EXPECT_STREQ("hello", handler(cmd));
}

TEST(handlerTest, Test2) {
  string cmd = "get teacher";
  EXPECT_STREQ("hello", handler(cmd));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

