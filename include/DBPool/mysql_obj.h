// Copyright 
// License
// Author: adairjun
// This is used to construct a mysql connection DAO(data access obj) 

#ifndef DBPOOL_INCLUDE_MYSQL_OBJ_H_
#define DBPOOL_INCLUDE_MYSQL_OBJ_H_

#include "QueryResult.h"
#include "debug.h"
#include "logobj.h"
#include <mysql/mysql.h>
#include <boost/shared_ptr.hpp>

// 设置一个标志,函数参数列表当中带有OUT的就是输出引用,带有IN的就是输入参数
#define OUT
#define IN

// MysqlObj通过MysqlObj(ip, user, passwd, db, port)来构造,通过Connect()来连接到Mysql数据库
// 通过ExecuteSql(vecResult, "select * from student")这种形式来执行sql,并把结果保存在vecResult当中
class MysqlObj {
 public:
  explicit MysqlObj(string HostIP, string pUser, string pPassword, string pDBname, unsigned iPort);
  virtual ~MysqlObj();
  MysqlObj(const MysqlObj&) = delete;
  MysqlObj& operator=(const MysqlObj&) = delete;
  /**
   *  Dump指的是把class的private成员全部打印出来看一下
   */
  void Dump() const;
  
  string ErrorMessage();
  
  /**
   * 根据成员变量来建立MYSQL连接
   */
  bool Connect();
  bool Reconnect();
  void Close();
  
  int SelectDB(const char* pDatabase);
  int ExecuteSql(IN const char* pSql, OUT QueryResult& vecResult);
  
  /**
   * 执行SQL受到影响的行数
   */
  unsigned long long AffectedRows() const;
  unsigned long long InsertId() const;
  
  /**
   * 返回MYSQL的指针
   */
  MYSQL* Get() const;
  
  /**
   * 测试mysql服务器是否存活
   */
  bool Ping() const;

  // 事务支持操作
  /**
   * 开始事务
   */
  int StartTransaction();

  /**
   * 提交事务
   */
  int Commit();

  /**
   * 回滚事务
   */
  int RollBack();

 private:
  // MYSQL代表了一条TCP连接
  MYSQL* pMysql_;
  
  // 连接必备的参数:IP地址,用户,密码,数据库名称,端口号
  string strHost_;
  string strUser_;
  string strPassword_;
  string strDBname_;
  unsigned iPort_;
  
  //错误信息
  string strErrorMessage_;
  // 
  unsigned long long iAffectedRows_;
  unsigned long long iInsertId_;
};

// 使用shared_ptr来替换MysqlObj*
typedef boost::shared_ptr<MysqlObj>MysqlObjPtr;
#endif /* DBPOOL_INCLUDE_MYSQL_OBJ_H_ */
