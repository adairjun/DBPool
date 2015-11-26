#include "DBPool/mysql_obj.h"
#include <glog/logging.h>
#include <assert.h>
#include <cstring>


MysqlObj::MysqlObj(string host, string user, string password, string dbname, unsigned port)
	: strHost_(host), 
	  strUser_(user), 
	  strPassword_(password), 
	  strDBname_(dbname), 
	  iPort_(port) {
	pMysql_ = NULL;
}

MysqlObj::~MysqlObj() {	
  Close();
}

void MysqlObj::Dump() const { 
  printf("MysqlObj::Dump ========== \n");
  printf("pMysql_=%p ", pMysql_);
  printf("strHost_=%s ", strHost_.c_str());
  printf("strUser_=%s ", strUser_.c_str());
  printf("strPassword_=%s ", strPassword_.c_str());
  printf("strDBname_=%s ", strDBname_.c_str());
  printf("iPort_=%d ", iPort_);
  printf("strErrorMessage_=%s ", strErrorMessage_.c_str());
  printf("iAffectedRows_=%d ", iAffectedRows_);
  printf("iInsertId_=%d ", iInsertId_);
  printf("\n");
}

string MysqlObj::ErrorMessage() {
  strErrorMessage_ = "";
  if (pMysql_) {
    string pMessage = mysql_error(pMysql_);
    strErrorMessage_ = pMessage;
  }
  return strErrorMessage_;
}

bool MysqlObj::Connect() {
  pMysql_ = mysql_init(NULL);
  if (NULL == pMysql_) {
  	return false;
  }
  
  char cValue = 1;	
  if (0 != mysql_options(pMysql_, MYSQL_OPT_RECONNECT, (char *)&cValue))	{
    ErrorMessage();
    Close();
    return false;
  }
  
  if (NULL == mysql_real_connect(pMysql_, strHost_.c_str(), strUser_.c_str(), strPassword_.c_str(), strDBname_.c_str(), iPort_, NULL, 0)) {
    ErrorMessage();
    Close();
  	return false;
  }
  mysql_set_character_set(pMysql_, "utf8");
  return true;
}

bool MysqlObj::Reconnect() {
  Close();
  pMysql_ = mysql_init(NULL);
  if (NULL == pMysql_) {
  	return false;
  }
  if (NULL == mysql_real_connect(pMysql_, 
  			strHost_.size()?strHost_.c_str():NULL, 
  			strUser_.size()?strUser_.c_str():NULL, 
  			strPassword_.size()?strPassword_.c_str():NULL, 
  			strDBname_.size()?strDBname_.c_str():NULL,
  			iPort_, NULL, CLIENT_MULTI_STATEMENTS)) {
      ErrorMessage();
	  Close();
  	  return false;
  }
  return true;
}

void MysqlObj::Close() {
  if (pMysql_) {
  	mysql_close(pMysql_);
  	pMysql_ = NULL;
  }
}

int MysqlObj::SelectDB(const char *pDatabase) {
  assert(pMysql_);
  int iRet = mysql_select_db(pMysql_, pDatabase);
  if (0 != iRet) {
    ErrorMessage();
  }
  return iRet;
}

int MysqlObj::ExecuteSql(IN const char *pSql, OUT QueryResult& vecResult) {
  assert(pMysql_);
  unsigned int iSqlSize = (unsigned int)strlen(pSql);
  // 使用glog打日志
  LOG(INFO) << "[Before execute SQL: ]" << pSql;  
  if (0 != mysql_real_query(pMysql_, pSql, iSqlSize)) {
    ErrorMessage();
    return -1;
  }
  unsigned iFieldCount = 0;
  unsigned iRowCount = 0;
  
  if (0 == (iFieldCount = mysql_field_count(pMysql_))) {
    // 当前结果集无数据，说明不是SELECT的Sql
    iAffectedRows_ = mysql_affected_rows(pMysql_);
    iInsertId_ = mysql_insert_id(pMysql_);
  } else {
    // Select的结果集
    MYSQL_RES *pRes = mysql_store_result(pMysql_);
    if (NULL == pRes) {
        ErrorMessage();
        return -1;
    }
    
    // 获取行数
    iRowCount = mysql_num_rows(pRes);
    iAffectedRows_ = iRowCount;
    // 当前为Select，修正insert_id为0
    iInsertId_ = 0;
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
  while(!mysql_next_result(pMysql_)) {
	if (0 == mysql_field_count(pMysql_)) {
	  continue;
    }
    MYSQL_RES *pRes = mysql_store_result(pMysql_);
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
  return iAffectedRows_;
}

unsigned long long MysqlObj::InsertId() const {
  return iInsertId_;
}

MYSQL* MysqlObj::Get() const {
  return pMysql_;
}

bool MysqlObj::Ping() const {
  if(pMysql_ != NULL && !mysql_ping(pMysql_))
  	return true;
  else
  	return false;
}

int MysqlObj::StartTransaction() {
  if (0 != mysql_real_query(pMysql_, "START TRANSACTION", (unsigned long)strlen("START TRANSACTION"))) {
    ErrorMessage();
    return -1;
  } else {
    return 0;
  }
}

int MysqlObj::Commit() {
  if (0 != mysql_real_query(pMysql_, "COMMIT", (unsigned long)strlen("COMMIT"))) {
    ErrorMessage();
    return -1;
  } else {
    return 0;
  }
  
}

int MysqlObj::RollBack() {
  if (0 != mysql_real_query(pMysql_, "ROLLBACK", (unsigned long)strlen("ROLLBACK"))) {
    ErrorMessage();
    return -1;
  } else {
    return 0;
  }
  
}
