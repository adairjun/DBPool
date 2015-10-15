#include "DBPool/redis_connection_pool.h"
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using std::cout;
using std::endl;

RedisPool::RedisPool() {
  // 从配置文件database.xml当中读入redis的ip, 用户, 密码, 和最大连接数	
  boost::property_tree::ptree pt;	
  const char* xml_path = "../config/database.xml";	
  boost::property_tree::read_xml(xml_path, pt);
  
  //这段注释的代码是读取json配置文件的
  //const char* json_path = "../config/database.json";
  //boost::property_tree::read_json(json_path, pt);

  BOOST_AUTO(child, pt.get_child("Config.RedisConnection"));
  for (BOOST_AUTO(pos, child.begin()); pos!= child.end(); ++pos) {
  	if (pos->first == "IP") host_ = pos->second.data();
  	if (pos->first == "Port") port_ = boost::lexical_cast<int>(pos->second.data());
  	if (pos->first == "Passwd") password_ = pos->second.data();
  	if (pos->first == "max_connections") poolSize_ = boost::lexical_cast<int>(pos->second.data());
  }
  // 构造函数的作用就是根据poolSize的大小来构造多个映射
  // 每个映射的连接都是同样的host,pass,dbname
  
  for (int i=0; i<poolSize_; ++i) {
  	// 这里使用智能指针RedisObjPtr
  	RedisObjPtr conn(new RedisObj(host_, password_, port_));
  	unique_lock<mutex> lk(resource_mutex);
  	if (conn->Connect()) {
  	  redis_list.push_back(conn);
  	} else {
  	  m_strErrorMessage = conn->ErrorMessage();
  	}
  }

}

RedisPool::~RedisPool() {
	// 由于使用了智能指针,不需要手动delete,析构函数不需要操作
}

// 从list当中得到一个连接
RedisObjPtr RedisPool::GetConnection() {
  //get connection operation
  unique_lock<mutex> lk(resource_mutex);
  if (!redis_list.empty()) { 
    RedisObjPtr ret = redis_list.front();
    redis_list.pop_front();
    return ret;
  }
}

// 释放一个连接还给线程池
int RedisPool::ReleaseConnection(RedisObjPtr conn) {
  if (conn) {
    redis_list.push_back(conn);
  }
  return 1;
}

string RedisPool::ErrorMessage() const {
  return m_strErrorMessage; 
}
