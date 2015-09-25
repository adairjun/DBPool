#include "../include/redis_obj.h"
#include <assert.h>
#include <cstring>

RedisObj::RedisObj(string host, string password, unsigned port)
: r_strHost(host), r_strPassword(password), r_iPort(port)
{
	r_pRedis = NULL;
}

RedisObj::~RedisObj()
{
	Close();
}

void RedisObj::Dump() const
{
	printf("r_pRedis=%p", r_pRedis);
}

string RedisObj::ErrorMessage()
{
    return "";
}

bool RedisObj::Connect()
{
	r_pRedis = redisConnect(r_strHost.c_str(), r_iPort);  
	if ( r_pRedis->err )
	{
		return false;
	}
	return true;
}

void RedisObj::Close() 
{
	if ( r_pRedis ) 
	{
		redisFree(r_pRedis);
		r_pRedis = NULL;
	}
}

redisContext* RedisObj::get() const
{
	return r_pRedis;
}

// 从redis当中获取返回值
int RedisObj::IntegerResult(OUT long long &result, IN redisReply* reply)
{
    if(reply == NULL)
        return -1;
    if(reply->type != REDIS_REPLY_INTEGER)
        return -1;

    result = reply->integer;
    return 1;
}

int RedisObj::StringResult(OUT std::string &result, IN redisReply* reply)
{
    if(reply == NULL)
        return -1;
    if(reply->type == REDIS_REPLY_NIL)
        return 0;
    if(reply->type != REDIS_REPLY_STRING)
        return -1;

    result.clear();
    result.assign(reply->str, reply->len);
    return 1;
}

int RedisObj::StatusResult(OUT std::string &result, IN redisReply* reply)
{
    if(reply == NULL)
        return -1;
    if(reply->type != REDIS_REPLY_STATUS)
        return -1;

    result.clear();
    result.assign(reply->str, reply->len);
    return 1;
}

int RedisObj::StringArrayResult(OUT std::vector<std::string> &result, IN redisReply* reply)
{
    if(reply == NULL)
        return -1;
    if(reply->type == REDIS_REPLY_NIL)
        return 0;
    if(reply->type != REDIS_REPLY_ARRAY)
        return -1;

    result.clear();
    for(size_t i=0; i<reply->elements; i++)
    {
        redisReply* r = reply->element[i];
        if(r->type == REDIS_REPLY_STRING)
        {
            result.emplace_back(r->str, r->len);
        }
    }
    if(result.size() == 0)
        return 0;
    return 1;
}
int RedisObj::ArrayResult(OUT std::vector<redisReply*> &result, IN redisReply* reply)
{
    if(reply == NULL)
        return -1;
    if(reply->type != REDIS_REPLY_ARRAY)
        return -1;

    result.clear();
    for(size_t i=0; i<reply->elements; i++)
    {
        redisReply* r = reply->element[i];
        result.emplace_back(r);
    }
    if(result.size() == 0)
        return 0;
    return 1;
}

