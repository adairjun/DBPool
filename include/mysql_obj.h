// Copyright 
// License
// Author: adairjun
// This is used to construct a mysql connection DAO(data access obj) 

#ifndef DBPOOL_INCLUDE_MYSQL_OBJ_H_
#define DBPOOL_INCLUDE_MYSQL_OBJ_H_

#include <mysql/mysql.h>
#include <boost/shared_ptr.hpp>
#include "QueryResult.h"
#include "debug.h"

// 设置一个标志,函数参数列表当中带有OUT的就是输出引用,带有IN的就是输入参数
#define OUT
#define IN

class MysqlObj
{
 public:
  MysqlObj(string HostIP, string pUser, string pPassword, string pDBname, unsigned iPort);
  virtual ~MysqlObj();
  // Dump指的是把m_pMysql的指针指向的值打印出来
  void Dump() const;
  
  string ErrorMessage();
  
  // 根据成员变量来建立MYSQL连接
  bool Connect();
  bool Reconnect();
  void Close();
  
  int SelectDB(const char* pDatabase);
  int ExecuteSql(IN const char* pSql, OUT QueryResult& vecResult);
  
  //执行SQL受到影响的行数
  unsigned long long AffectedRows() const;
  unsigned long long InsertId() const;
  
  //返回MYSQL的指针
  MYSQL* get() const;
  
  //测试mysql服务器是否存活
  bool Ping() const;

 private:
  // MYSQL代表了一条TCP连接
  MYSQL* m_pMysql;
  
  // 连接必备的参数:IP地址,用户,密码,数据库名称,端口号
  string m_strHost;
  string m_strUser;
  string m_strPassword;
  string m_strDBname;
  unsigned m_iPort;
  
  //错误信息
  string m_strErrorMessage;
  // 
  unsigned long long m_iAffectedRows;
  unsigned long long m_iInsertId;
};

// 使用shared_ptr来替换MysqlObj*
 typedef boost::shared_ptr<MysqlObj>MysqlObjPtr;
#endif /* DBPOOL_INCLUDE_MYSQL_OBJ_H_ */
