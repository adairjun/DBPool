#include "DBPool/mysql_connection_pool.h"
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

MysqlPool::MysqlPool() {
  // 从配置文件database.xml当中读入mysql的ip, 用户, 密码, 数据库名称,	
  boost::property_tree::ptree pt;	
  const char* xml_path = "../config/database.xml";	
  boost::property_tree::read_xml(xml_path, pt);
  
  //这段注释的代码是读取json配置文件的
  //	const char* json_path = "../config/database.json";
  //	boost::property_tree::read_json(json_path, pt);
  
  BOOST_AUTO(child, pt.get_child("Config.MysqlConnection"));
  for (BOOST_AUTO(pos, child.begin()); pos!= child.end(); ++pos) {
  	if (pos->first == "IP") host_ = pos->second.data();
  	if (pos->first == "Port") port_ = boost::lexical_cast<int>(pos->second.data());
  	if (pos->first == "User") user_ = pos->second.data();
  	if (pos->first == "Passwd") password_ = pos->second.data();
  	if (pos->first == "DBName") dbname_ = pos->second.data();
  	if (pos->first == "max_connections") poolSize_ = boost::lexical_cast<int>(pos->second.data());
  }
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
