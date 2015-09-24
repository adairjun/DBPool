#ifndef _MYSQL_OBJ_H
#define _MYSQL_OBJ_H

#include <mysql/mysql.h>
#include "QueryResult.h"

class MysqlObj
{
 public:
	MysqlObj(string HostIP, string pUser, string pPassword, string pDBname, unsigned iPort);
	virtual ~MysqlObj();
	// Dump指的是把m_pMysql的指针指向的值打印出来
	void Dump() const;

	string ErrorMessage();

	bool Connect();
	bool Reconnect();
	void Close();
	
	int SelectDB(const char* pDatabase);
	int ExecuteSql(const char* pSql, QueryResult& vecResult);

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

#endif /* MYSQL_OBJ_H */
