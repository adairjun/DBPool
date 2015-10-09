// Copyright 
// License
// Author: adairjun
// This is used to construct a mysql connection pool 

#ifndef DBPOOL_INCLUDE_MYSQL_CONNECTION_POOL_H_
#define DBPOOL_INCLUDE_MYSQL_CONNECTION_POOL_H_

#include <string.h>
#include <mutex>
#include <map>
#include "mysql_obj.h"

using std::string;
using std::vector;
using std::mutex;
using std::unique_lock;
using std::map;
using std::make_pair;

// 这个pool只做了一件事情,那就是创建poolsize个的连接,Mysql的最大连接数目是151
class MysqlPool
{
 public:
  // 批量创建连接在构造函数当中进行
  MysqlPool();
  // 析构函数就是顺序销毁MysqlObj指针
  // 最后清空map
  virtual ~MysqlPool();
  	
  // 从map当中选取一个连接
  MysqlObj* getConnection();
  
  // 释放特定的连接就是把map当中的bool值置为false
  int releaseConnection(MysqlObj*);
  
  // 构造函数创建poolsize个连接错误时候用来打印错误信息
  string ErrorMessage() const;
  
 private:
  mutex resource_mutex;

 public:
  // 之所以要把这里设置成为public,是因为使用insert的方式来插入连接和是否可用的pair
  // bool值为false的时候说明mysql连接被占用
  map<MysqlObj*, bool> mysql_map;
  //===========================================================
  // 这里或许可以使用list来代替map,当要取出连接的时候使用list的front和pop,释放连接的时候使用list的push
  //===========================================================

 private:
  string host_ ;
  string user_ ;
  string password_ ;
  string dbname_;
  unsigned port_;
  
  //Mysql的最大连接数为151,从database.xml或者database.json当中读取max_connections
  int poolSize_;
  
  //错误信息
  string m_strErrorMessage;  

};

#endif /* DBPOOL_INCLUDE_MYSQL_CONNECTION_POOL_H_ */
