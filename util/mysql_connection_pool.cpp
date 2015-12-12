#include "DBPool/mysql_connection_pool.h"
#include "DBPool/parse_xml.h"
#include "DBPool/parse_json.h"
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <boost/lexical_cast.hpp>

using std::cout;
using std::endl;

MysqlPool::MysqlPool() {
  ParseXmlObj myXml("../config/database.xml");
  map<string, string> key_value_map = myXml.GetChildDataMap("Config.MysqlConnection"); 
  //这段注释的代码是读取json配置文件的
  // ParseJsonObj myJson("../config/database.json");
  // map<string, string> key_value_map = myJson.GetChildDataMap("Config.MysqlConnection"); 
   host_ = key_value_map["IP"];
   port_ = boost::lexical_cast<int>(key_value_map["Port"]);
   user_ = key_value_map["User"];
   password_ = key_value_map["Passwd"];
   dbname_ = key_value_map["DBName"];
   poolSize_ = boost::lexical_cast<int>(key_value_map["max_connections"]);
  
  // 构造函数的作用就是根据poolSize的大小来构造多个映射
  // 每个映射的连接都是同样的host,user,pass,dbname
  
  for (int i=0; i<poolSize_; ++i) {
  	MysqlObjPtr conn(new MysqlObj(host_, user_, password_, dbname_, port_));
  	if (conn->Connect()) {
  	  mysql_list.push_back(conn);
  	} else {
  	  strErrorMessage_ = conn->ErrorMessage();
  	}
  }
}

MysqlPool::~MysqlPool() {
}

void MysqlPool::Dump() const{
  printf("\n=====MysqlPool Dump START ========== \n");
  printf("host_=%s ", host_.c_str());
  printf("user_=%s ", user_.c_str());
  printf("password_=%s ", password_.c_str());
  printf("dbname_=%s ", dbname_.c_str());
  printf("port_=%d ", port_);
  printf("poolSize_=%d ", poolSize_);
  printf("strErrorMessage_=%s ", strErrorMessage_.c_str());
  //至于list<MysqlObjPtr> mysql_list这个成员
  int count = 0;
  for (auto it = mysql_list.begin(); it!=mysql_list.end(); ++it) {
    printf("count==%d ", count);
    (*it)->Dump();
    ++count;
  }
  printf("\n===MysqlPool DUMP END ============\n");
}

bool MysqlPool::Empty() const {
  return mysql_list.empty();
}

/**
 * 从map当中得到一个连接
 */
MysqlObjPtr MysqlPool::GetConnection() {
  //get connection operation
  unique_lock<mutex> lk(resource_mutex);
  if (!mysql_list.empty()) {
    MysqlObjPtr ret = mysql_list.front();
    mysql_list.pop_front();
    return ret;
  }  
}

/**
 * 释放一个连接还给线程池
 */
int MysqlPool::ReleaseConnection(MysqlObjPtr conn) {
  unique_lock<mutex> lk(resource_mutex);
  if (conn) {
    mysql_list.push_back(conn);
  }
  return 1;
}

string MysqlPool::ErrorMessage() const {
  return strErrorMessage_; 
}
