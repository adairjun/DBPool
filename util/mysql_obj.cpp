#include "DBPool/mysql_obj.h"
#include <assert.h>
#include <cstring>

MysqlObj::MysqlObj(string host, string user, string password, string dbname, unsigned port)
	: m_strHost(host), 
	  m_strUser(user), 
	  m_strPassword(password), 
	  m_strDBname(dbname), 
	  m_iPort(port) {
	m_pMysql = NULL;
}

MysqlObj::~MysqlObj() {	
  Close();
}

void MysqlObj::Dump() const {
  printf("m_pMysql=%p", m_pMysql);
}

string MysqlObj::ErrorMessage() {
  m_strErrorMessage = "";
  if (m_pMysql) {
    string pMessage = mysql_error(m_pMysql);
    m_strErrorMessage = pMessage;
  }
  return m_strErrorMessage;
}

bool MysqlObj::Connect() {
  m_pMysql = mysql_init(NULL);
  if (NULL == m_pMysql) {
  	return false;
  }
  
  char cValue = 1;	
  if (0 != mysql_options(m_pMysql, MYSQL_OPT_RECONNECT, (char *)&cValue))	{
    ErrorMessage();
    Close();
    return false;
  }
  
  if (NULL == mysql_real_connect(m_pMysql, m_strHost.c_str(), m_strUser.c_str(), m_strPassword.c_str(), m_strDBname.c_str(), m_iPort, NULL, 0)) {
    ErrorMessage();
    Close();
  	return false;
  }
  mysql_set_character_set(m_pMysql, "gb2312");
  return true;
}

bool MysqlObj::Reconnect() {
  Close();
  m_pMysql = mysql_init(NULL);
  if (NULL == m_pMysql) {
  	return false;
  }
  if (NULL == mysql_real_connect(m_pMysql, 
  			m_strHost.size()?m_strHost.c_str():NULL, 
  			m_strUser.size()?m_strUser.c_str():NULL, 
  			m_strPassword.size()?m_strPassword.c_str():NULL, 
  			m_strDBname.size()?m_strDBname.c_str():NULL,
  			m_iPort, NULL, CLIENT_MULTI_STATEMENTS)) {
      ErrorMessage();
	  Close();
  	  return false;
  }
  return true;
}

void MysqlObj::Close() {
  if (m_pMysql) {
  	mysql_close(m_pMysql);
  	m_pMysql = NULL;
  }
}

int MysqlObj::SelectDB(const char *pDatabase) {
  assert(m_pMysql);
  int iRet = mysql_select_db(m_pMysql, pDatabase);
  if (0 != iRet) {
    ErrorMessage();
  }
  return iRet;
}

int MysqlObj::ExecuteSql(IN const char *pSql, OUT QueryResult& vecResult) {
  assert(m_pMysql);
  unsigned int iSqlSize = (unsigned int)strlen(pSql);
  
  if (0 != mysql_real_query(m_pMysql, pSql, iSqlSize)) {
    ErrorMessage();
    return -1;
  }
  unsigned iFieldCount = 0;
  unsigned iRowCount = 0;
  
  if (0 == (iFieldCount = mysql_field_count(m_pMysql))) {
    // 当前结果集无数据，说明不是SELECT的Sql
    m_iAffectedRows = mysql_affected_rows(m_pMysql);
    m_iInsertId = mysql_insert_id(m_pMysql);
  } else {
    // Select的结果集
    MYSQL_RES *pRes = mysql_store_result(m_pMysql);
    if (NULL == pRes) {
        ErrorMessage();
        return -1;
    }
    
    // 获取行数
    iRowCount = mysql_num_rows(pRes);
    m_iAffectedRows = iRowCount;
    // 当前为Select，修正insert_id为0
    m_iInsertId = 0;
    MYSQL_ROW pRow = NULL;
    for (unsigned i = 0; i < iRowCount; ++i) {
      pRow = mysql_fetch_row(pRes);
      if (!pRow) {
        ErrorMessage();
        mysql_free_result(pRes);
        return -1;
      }
      unsigned long *pLens = mysql_fetch_lengths(pRes); 
      if (NULL == pLens) {
        ErrorMessage();
        mysql_free_result(pRes);
        return -1;
      }
      
      // 构造一行
      vector<string> vecRow;
      for (unsigned j = 0;j < iFieldCount;++j) {
		vecRow.push_back(string(pRow[j], pLens[j]));
      }
      vecResult.addRow(vecRow);
    }
    // 释放当前结果集
    mysql_free_result(pRes);
  }
  
  // XXX:只保留第一个结果集，后续的忽略
  while(!mysql_next_result(m_pMysql)) {
	if (0 == mysql_field_count(m_pMysql)) {
	  continue;
    }
    MYSQL_RES *pRes = mysql_store_result(m_pMysql);
    if (NULL == pRes) {
      ErrorMessage();
      return -1;
    }
    // 释放当前结果集
    mysql_free_result(pRes);
  }
  
  return 0;
}

unsigned long long MysqlObj::AffectedRows() const {
  return m_iAffectedRows;
}

unsigned long long MysqlObj::InsertId() const {
  return m_iInsertId;
}

MYSQL* MysqlObj::Get() const {
  return m_pMysql;
}

bool MysqlObj::Ping() const {
  if(m_pMysql != NULL && !mysql_ping(m_pMysql))
  	return true;
  else
  	return false;
}

int MysqlObj::StartTransaction() {
  if (0 != mysql_real_query(m_pMysql, "START TRANSACTION", (unsigned long)strlen("START TRANSACTION"))) {
    ErrorMessage();
    return -1;
  } else {
    return 0;
  }
}

int MysqlObj::Commit() {
  if (0 != mysql_real_query(m_pMysql, "COMMIT", (unsigned long)strlen("COMMIT"))) {
    ErrorMessage();
    return -1;
  } else {
    return 0;
  }
  
}

int MysqlObj::RollBack() {
  if (0 != mysql_real_query(m_pMysql, "ROLLBACK", (unsigned long)strlen("ROLLBACK"))) {
    ErrorMessage();
    return -1;
  } else {
    return 0;
  }
  
}
