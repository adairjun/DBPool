// Copyright 
// License
// Author: adairjun
// This is used to construct a mysql connection pool 

#ifndef DBPOOL_INCLUDE_MYSQL_CONNECTION_POOL_H_
#define DBPOOL_INCLUDE_MYSQL_CONNECTION_POOL_H_

#include <string.h>
#include <mutex>
#include <list>
#include "mysql_obj.h"

using std::string;
using std::vector;
using std::mutex;
using std::unique_lock;
using std::list;

// 这个pool只做了一件事情,那就是创建poolsize个的连接,Mysql的最大连接数目是151
class MysqlPool {
 public:
  /**
    * 批量创建连接在构造函数当中进行
    */
  explicit MysqlPool();
  /**
   * 析构函数就是顺序销毁MysqlObj指针
   * 最后清空map
   */
  virtual ~MysqlPool();

  MysqlPool(const MysqlPool&) = delete;  	
  MysqlPool& operator=(const MysqlPool&) = delete;

  void Dump() const;
  
  bool Empty() const;
  /**
   * 从map当中选取一个连接
   */
  MysqlObjPtr GetConnection();
  
  /**
   * 释放特定的连接就是把map当中的bool值置为false
   */
  int ReleaseConnection(MysqlObjPtr);
  
  /**
   * 构造函数创建poolsize个连接错误时候用来打印错误信息
   */
  string ErrorMessage() const;
  
 private:
  mutex resource_mutex;

 public:
  // 本来是使用map来保存连接的,改为用list效率更佳
  list<MysqlObjPtr> mysql_list;

 private:
  string host_ ;
  string user_ ;
  string password_ ;
  string dbname_;
  unsigned port_;
  
  //Mysql的最大连接数为151,从database.xml或者database.json当中读取max_connections
  int poolSize_;
  
  //错误信息
  string strErrorMessage_;  

};

// 使用shared_ptr来替换MysqlPool*
typedef boost::shared_ptr<MysqlPool>MysqlPoolPtr;
#endif /* DBPOOL_INCLUDE_MYSQL_CONNECTION_POOL_H_ */
