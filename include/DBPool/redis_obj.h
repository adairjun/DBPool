// Copyright 
// License
// Author: adairjun
// This is used to construct a redis connection DAO(data access obj) 

#ifndef _REDIS_OBJ_H
#define _REDIS_OBJ_H

#include "logobj.h"
#include <hiredis/hiredis.h>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

using std::string;
using std::vector;

// 设置一个标志,函数参数列表当中带有OUT的就是输出引用,带有IN的就是输入参数
#define OUT
#define IN

// RedisObj通过RedisObj(ip, passwd, port)来构造,通过Connect()来连接到数据库
// 通过执行ExecuteCmd("get student")将redis查询的结果保存在pResult_当中
class RedisObj {
 public:
  explicit RedisObj(string HostIP, string pPassword, unsigned iPort);
  virtual ~RedisObj();
  
  RedisObj(const RedisObj&) = delete;
  RedisObj& operator=(const RedisObj&) = delete;

  /**
   * Dump指的是把所有的成员函数都打印出来
   */
  void Dump() const;
  
  string ErrorMessage();
  
  /**
   * 根据成员变量来建立Redis连接
   */
  bool Connect();
  void Close();
  
  int ExecuteCmd(const char* pCmd);
  
  /**
   * 返回Redis的指针
   */
  redisContext* Get() const;

 public: 
  /**
   * 从redis当中获取返回值
   */
  int IntegerResult(OUT long long &result);
  int StringResult(OUT string &result);
  int StatusResult(OUT string &result);
  int StringArrayResult(OUT vector<string> &result);
  int ArrayResult(OUT vector<redisReply*> &result);
  
 private:
  /**
   * 从redis当中获取返回值
   */
  int IntegerResult(IN redisReply* reply, OUT long long &result);
  int StringResult(IN redisReply* reply, OUT string &result);
  int StatusResult(IN redisReply* reply, OUT string &result);
  int StringArrayResult(IN redisReply* reply, OUT vector<std::string> &result);
  int ArrayResult(IN redisReply* reply, OUT vector<redisReply*> &result);
  
 private:
  // 代表了一条数据库连接
  redisContext* pRedis_;
  redisReply* pResult_;
 private:
  // 连接必备的参数:IP地址,用户,密码,数据库名称,端口号
  string strHost_;
  string strPassword_;
  unsigned iPort_;
  
  //错误信息
  string strErrorMessage_;
};

// 尝试使用shared_ptr来代替RedisObj*
typedef boost::shared_ptr<RedisObj>RedisObjPtr;

#endif /* REDIS_OBJ_H */
