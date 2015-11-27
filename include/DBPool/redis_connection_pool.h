// Copyright 
// License
// Author: adairjun
// This is used to construct a redis connection pool 

#ifndef DBPOOL_INCLUDE_REDIS_CONNECTION_POOL_H_
#define DBPOOL_INCLUDE_REDIS_CONNECTION_POOL_H_

#include <string.h>
#include <mutex>
#include <list>
#include "redis_obj.h"

using std::string;
using std::vector;
using std::mutex;
using std::unique_lock;
using std::list;

// 这个pool只做了一件事情,那就是创建poolsize个的连接
class RedisPool {
 public:
	/**
	 * 批量创建连接在构造函数当中进行
	 */
  explicit RedisPool();
  /**
   * 这里析构函数没有做任何事情
   */
  virtual ~RedisPool();
  
  RedisPool(const RedisPool&) = delete;
  RedisPool& operator=(const RedisPool&) = delete;
 
  void Dump() const;
  
  bool Empty() const;  	
  /**
   * 从list当中选取一个连接,pop出来。使用list的好处是每次需要取连接的时候只要取front,不需要像map一样去遍历,节约了时间
   */
  RedisObjPtr GetConnection();
  
  /**
   * 释放特定的连接就是把连接重新放回到list当中去
   */
  int ReleaseConnection(RedisObjPtr);
  
  /**
   * 构造函数创建poolsize个连接错误时候用来打印错误信息
   */
  string ErrorMessage() const;

 private:
  mutex resource_mutex;

 public:
  // 这里与mysql的连接池不一样,我想尝试使用list来保存连接
  list<RedisObjPtr> redis_list;

 private:
  string host_ ;
  string user_ ;
  string password_ ;
  unsigned port_;
  
  //从database.xml或者database.json当中读取max_connections
  int poolSize_;
  
  //错误信息
  string strErrorMessage_;  
};

// 使用shared_ptr来替换RedisPool*
typedef boost::shared_ptr<RedisPool>RedisPoolPtr;
#endif /* DBPOOL_INCLUDE_REDIS_CONNECTION_POOL_H_ */
