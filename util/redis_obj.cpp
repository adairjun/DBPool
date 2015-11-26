#include "DBPool/redis_obj.h"
#include <assert.h>
#include <cstring>

RedisObj::RedisObj(string host, string password, unsigned port)
	: strHost_(host), 
	  strPassword_(password), 
	  iPort_(port) {
  pRedis_ = NULL;
}

RedisObj::~RedisObj() {
  Close();
}

void RedisObj::Dump() const {
  printf("\n=====RedisObj Dump START ========== \n");
  printf("pRedis_=%p ", pRedis_);
  printf("pResult_=%p ", pResult_);
  printf("strHost_=%s ", strHost_.c_str());
  printf("strPassword_=%s ", strPassword_.c_str());
  printf("iPort_=%d ", iPort_);
  printf("strErrorMessage_=%s ", strErrorMessage_.c_str());
  printf("\n===RedisObj DUMP END ============\n");
}

string RedisObj::ErrorMessage() {
  return strErrorMessage_;
}

bool RedisObj::Connect() {
  pRedis_ = redisConnect(strHost_.c_str(), iPort_);  
  if (pRedis_->err) {
    strErrorMessage_ = "pRedis->err";
    return false;
  }
  return true;
}

void RedisObj::Close() {
  if (pRedis_) {
    redisFree(pRedis_);
    pRedis_ = NULL;
  }
}

int RedisObj::ExecuteCmd(IN const char* pCmd) {
  pResult_ = (redisReply*)redisCommand(pRedis_, pCmd);  
  if(pResult_ == NULL) {
    strErrorMessage_ = "pResult == NULL";
    return -1;
  }  
  return 0;
}


redisContext* RedisObj::Get() const {
  return pRedis_;
}


int RedisObj::IntegerResult(OUT long long &result) {
  return IntegerResult(pResult_, result);
}

int RedisObj::StringResult(OUT string &result) {
  return StringResult(pResult_, result);
}

int RedisObj::StatusResult(OUT string &result) {
  return StatusResult(pResult_, result);
}

int RedisObj::StringArrayResult(OUT vector<string> &result) {
  return StringArrayResult(pResult_, result);
}

int RedisObj::ArrayResult(OUT vector<redisReply*> &result) {
  return ArrayResult(pResult_, result);
}

// 从redis当中获取返回值
int RedisObj::IntegerResult(IN redisReply* reply, OUT long long &result) {
  if (reply == NULL) return -1;
  if (reply->type != REDIS_REPLY_INTEGER) return -1;

  result = reply->integer;
  return 0;
}

int RedisObj::StringResult(IN redisReply* reply, OUT std::string &result) {
  if (reply == NULL) return -2;
  if (reply->type == REDIS_REPLY_NIL) return -1;
  if (reply->type != REDIS_REPLY_STRING) return -2;

  result.clear();
  result.assign(reply->str, reply->len);
  return 0;
}

int RedisObj::StatusResult(IN redisReply* reply, OUT std::string &result) {
  if (reply == NULL) return -1;
  if (reply->type != REDIS_REPLY_STATUS) return -1;

  result.clear();
  result.assign(reply->str, reply->len);
  return 0;
}

int RedisObj::StringArrayResult(IN redisReply* reply, OUT std::vector<std::string> &result) {
  if (reply == NULL) return -2;
  if (reply->type == REDIS_REPLY_NIL) return -1;
  if (reply->type != REDIS_REPLY_ARRAY) return -2;

  result.clear();
  for (size_t i=0; i<reply->elements; i++) {
    redisReply* r = reply->element[i];
    if (r->type == REDIS_REPLY_STRING) {
      result.emplace_back(r->str, r->len);
    }
  }
  if (result.size() == 0) return -1;
  return 0;
}

int RedisObj::ArrayResult(IN redisReply* reply, OUT std::vector<redisReply*> &result) {
  if (reply == NULL) return -2;
  if (reply->type != REDIS_REPLY_ARRAY) return -2;

  result.clear();
  for (size_t i=0; i<reply->elements; i++) {
    redisReply* r = reply->element[i];
    result.emplace_back(r);
  }
  if(result.size() == 0) return -1;
  return 0;
}

