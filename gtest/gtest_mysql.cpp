#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <thread>
#include <Theron/Theron.h>
#include <gtest/gtest.h>
#include "DBPool/mysql_connection_pool.h"

using std::cout;
using std::endl;

// 初始化全局的msql连接池,下一步是启动多个actor,每个actor从连接池当中获取一个连接,然后并行地执行命令
static MysqlPoolPtr mysqlpool_ptr(new MysqlPool);

// Theron的使用方法一个actor对象必须继承自Theron::Actor
class Actor : public Theron::Actor {
 public:
  Actor(Theron::Framework &framework) : Theron::Actor(framework) {
    // 将Actor::Handler注册
    RegisterHandler(this, &Actor::Handler);
    // 可以在这里再注册一个函数 RegisterHandler(this, &Actor::helloworld),顺序注册的话就会顺序执行
  }

 private:
  // 定义注册的函数,第一个参数是接受的message的类型,第二个参数是发送message的源地址
  // 由于这里要操作sql,所以使用string类型
  void Handler(const string &sql, const Theron::Address from) {
    // 从全局的mysql连接池当中取出一个连接 
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
    // 把一个message发送回去,这里由于源地址使用了Theron::Receiver,而Receiver接受消息的类型就是int,所以随便发送个0
    Send(0, from);
  } 
};

// 这个函数也可以作为main函数,但是为了使用gtest做测试,所以写在这里
const char* select(const string &sql) {
  Theron::Receiver receiver;
  Theron::Framework framework;
  // 这里两个actor
  Actor actor(framework);
  Actor actor2(framework);
  // 说明消息是从receiver发送给actor,发送给actor之后马上就返回,不会阻塞在这里影响下面发送给actor2的操作
  framework.Send(sql, receiver.GetAddress(), actor.GetAddress());
  framework.Send(sql, receiver.GetAddress(), actor2.GetAddress());
  // actor和actor2将并行执行 
  receiver.Wait();
  return "hello"; 
}

TEST(selectTest, Test1) {
  // 测试用例就是一条查询的sql语句
  string sql = "SELECT * FROM student;";
  EXPECT_STREQ("hello", select(sql));
}

// 发送的Actor
class SendActor : public Theron::Actor {
 public:
  SendActor(Theron::Framework &framework) : Theron::Actor(framework) {
    // 将Actor::Handler注册
    RegisterHandler(this, &SendActor::Handler);
    // 可以在这里再注册一个函数 RegisterHandler(this, &Actor::helloworld),顺序注册的话就会顺序执行
  }

 private:
  // 定义注册的函数,第一个参数是接受的message的类型,第二个参数是发送message的源地址
  // 由于这里要操作sql,所以使用string类型
  void Handler(const string &sql, const Theron::Address from) {
    // 从全局的mysql连接池当中取出一个连接
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
    // 把一个message发送回去,这里由于源地址使用了ReceiverActor,而Receiver接受消息的类型就是string
    Send(queryResult.getElement(0,0), from);
  } 
};
// 新的尝试,构建一个receiver
class ReceiverActor : public Theron::Actor {
 public:
  ReceiverActor(Theron::Framework &framework) : Theron::Actor(framework) {
    RegisterHandler(this, &ReceiverActor::Handler);
  }
  string Get() { return result; }

 private:
  void Handler(const string &receiveMes, const Theron::Address from) {
   // 很简单的操作,就是把Actor发给ReceiverActor的消息保存起来
   result = receiveMes; 
  }
  string result;
}; 

const char* update(const string &sql) {
  Theron::Framework framework;
  SendActor sendActor(framework);
  ReceiverActor receiverActor(framework);
  framework.Send(sql, receiverActor.GetAddress(), sendActor.GetAddress());
  
  //使用sleep来避免主线程提前结束
  sleep(10);
  return receiverActor.Get().c_str();
}

TEST(updateTest, Test1) {
  string sql = "SELECT student_name FROM student WHERE student_id=1;";
  EXPECT_STREQ("xiongjun",update(sql));
}

int main(int argc, char** argv) {
  // 启动glog日志库
  google::InitGoogleLogging(argv[0]);
  google::SetLogDestination(google::INFO,"../log/INFO_");
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
