#include "DBPool/redis_connection_pool.h"
#include "DBPool/parse_xml.h"
#include "DBPool/parse_json.h"
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <boost/lexical_cast.hpp>

using std::cout;
using std::endl;

RedisPool::RedisPool() {
  ParseXmlObj myXml("../config/database.xml");
  map<string, string> key_value_map = myXml.GetChildDataMap("Config.MysqlConnection"); 
  //这段注释的代码是读取json配置文件的
  // ParseJsonObj myJson("../config/database.json");
  // map<string, string> key_value_map = myJson.GetChildDataMap("Config.MysqlConnection"); 
   host_ = key_value_map["IP"];
   port_ = boost::lexical_cast<int>(key_value_map["Port"]);
   password_ = key_value_map["Passwd"];
   poolSize_ = boost::lexical_cast<int>(key_value_map["max_connections"]);
  
  // 构造函数的作用就是根据poolSize的大小来构造多个映射
  // 每个映射的连接都是同样的host,pass,dbname
  
  for (int i=0; i<poolSize_; ++i) {
  	// 这里使用智能指针RedisObjPtr
  	RedisObjPtr conn(new RedisObj(host_, password_, port_));
  	unique_lock<mutex> lk(resource_mutex);
  	if (conn->Connect()) {
  	  redis_list.push_back(conn);
  	} else {
  	  strErrorMessage_ = conn->ErrorMessage();
  	}
  }

}

RedisPool::~RedisPool() {
	// 由于使用了智能指针,不需要手动delete,析构函数不需要操作
}

void RedisPool::Dump() const {
  printf("\n=====RedisPool Dump START ========== \n");
  printf("host_=%s ", host_.c_str());
  printf("user_=%s ", user_.c_str());
  printf("password_=%s ", password_.c_str());
  printf("port_=%d ", port_);
  printf("poolSize_=%d ", poolSize_);
  printf("strErrorMessage_=%s ", strErrorMessage_.c_str());
  int count = 0;
  for (auto it = redis_list.begin(); it!=redis_list.end(); ++it) {
    printf("count==%d ", count);
    (*it)->Dump();
    ++count;
  }
  printf("\n===RedisPool DUMP END ============\n");
}

bool RedisPool::Empty() const {
  return redis_list.empty();
}

/**
 * 从list当中得到一个连接
 */
RedisObjPtr RedisPool::GetConnection() {
  //get connection operation
  unique_lock<mutex> lk(resource_mutex);
  if (!redis_list.empty()) { 
    RedisObjPtr ret = redis_list.front();
    redis_list.pop_front();
    return ret;
  }
}

/**
 * 释放一个连接还给线程池
 */
int RedisPool::ReleaseConnection(RedisObjPtr conn) {
  if (conn) {
    redis_list.push_back(conn);
  }
  return 1;
}

string RedisPool::ErrorMessage() const {
  return strErrorMessage_; 
}
