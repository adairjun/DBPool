#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <thread>
#include <Theron/Theron.h>
#include <gtest/gtest.h>
#include "mysql_connection_pool.h"

using std::cout;
using std::endl;
using std::thread;

static MysqlPoolPtr mysqlpool_ptr(new MysqlPool);

class Actor : public Theron::Actor {
 public:
  Actor(Theron::Framework &framework) : Theron::Actor(framework) {
    RegisterHandler(this, &Actor::Handler);
  }

 private:
  void Handler(const string &sql, const Theron::Address from) {
	MysqlObjPtr conn = mysqlpool_ptr->GetConnection();
    QueryResult queryResult;
    conn->ExecuteSql(sql.c_str(), queryResult);
    mysqlpool_ptr->ReleaseConnection(conn);
    for (int i=0;i<queryResult.getRowCount();i++) {
      for(int j=0;j<queryResult.getColumnCount();j++) {
    	cout << queryResult.getElement(i,j) << " ";
      }
    	cout << endl;
    }
    Send(0, from);
  } 
};

const char* handler(const string &sql) {
  Theron::Receiver receiver;
  Theron::Framework(framework);
  Actor actor(framework);
  framework.Send(sql, receiver.GetAddress(), actor.GetAddress());
  receiver.Wait();
  return "hello"; 
}

TEST(handlerTest, Test1) {
  string sql = "SELECT * FROM student;";
  EXPECT_STREQ("hello", handler(sql));
}

TEST(handlerTest, Test2) {
  string sql = "INSERT INTO student VALUES(10,\"dddddd\");";
  EXPECT_STREQ("hello", handler(sql));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
