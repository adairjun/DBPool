#include <iostream>
#include <string>
#include <Theron/Theron.h>
#include "mysql_connection_pool.h"
#include <gtest/gtest.h>

using namespace std;

class Actor : public Theron::Actor {
 public:
  Actor(Theron::Framework &framework) : Theron::Actor(framework) {
    RegisterHandler(this, &Actor::Handler);
  }

 private:
  void Handler(const string &message, const Theron::Address from) {
    MysqlPoolPtr mysqlpool_ptr(new MysqlPool);
	MysqlObjPtr conn = mysqlpool_ptr->GetConnection();
    QueryResult queryResult;
    conn->ExecuteSql(message.c_str(), queryResult);
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

int main() {
  Theron::Receiver receiver;
  Theron::Framework framework;
  Actor actor(framework);
  Actor actor_two(framework);
  string str="SELECT * FROM student;";
  framework.Send(str, receiver.GetAddress(), actor.GetAddress());
  string str2="REPLACE INTO student VALUES(3,\"hangeng\");";
  framework.Send(str2, receiver.GetAddress(), actor_two.GetAddress());
  receiver.Wait();
  return 0;
}
