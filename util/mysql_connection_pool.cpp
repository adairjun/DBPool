#include "../include/mysql_connection_pool.h"
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using std::cout;
using std::endl;

MysqlPool::MysqlPool()
{
	// 从配置文件database.xml当中读入mysql的ip, 用户, 密码, 数据库名称,	
	boost::property_tree::ptree pt;	
	const char* xml_path = "../config/database.xml";	
	boost::property_tree::read_xml(xml_path, pt);
	
	//这段注释的代码是读取json配置文件的
//	const char* json_path = "../config/database.json";
//	boost::property_tree::read_json(json_path, pt);

	BOOST_AUTO(child, pt.get_child("Config.MysqlConnection"));
	for(BOOST_AUTO(pos, child.begin()); pos!= child.end(); ++pos)
	{
		if(pos->first == "IP")
			host_ = pos->second.data();
		if(pos->first == "Port")
			port_ = boost::lexical_cast<int>(pos->second.data());
		if(pos->first == "User")
			user_ = pos->second.data();
		if(pos->first == "Passwd")
			password_ = pos->second.data();
		if(pos->first == "DBName")
			dbname_ = pos->second.data();
		if(pos->first == "max_connections")
			poolSize_ = boost::lexical_cast<int>(pos->second.data());
	}
	// 构造函数的作用就是根据poolSize的大小来构造多个映射
	// 每个映射的连接都是同样的host,user,pass,dbname

	for(int i=0; i<poolSize_; ++i)
	{
//		boost::shared_ptr<Connection> conn =  
		
		MysqlObj* conn = new MysqlObj(host_, user_, password_, dbname_, port_);
		conn->Connect();
		if(conn->Connect())
			mysql_map.insert(make_pair(conn, 1));
		else
		{
			m_strErrorMessage = conn->ErrorMessage();
			delete conn;
		}
	}

}

MysqlPool::~MysqlPool()
{
	// 析构函数就是顺序销毁MysqlObj指针
	
	for(auto it = mysql_map.begin(); it != mysql_map.end(); ++it)
	{
		delete it->first;
	}
	mysql_map.clear();
}


// 从map当中得到一个连接
MysqlObj* MysqlPool::getConnection()
{
	//get connection operation
	MysqlObj* ret = NULL;
	while(true)
	{
		bool flag = false;
		for(auto it = mysql_map.begin(); it != mysql_map.end(); ++it)
		{
			unique_lock<mutex> lk(resource_mutex);
			if(it->second == true)
			{
				it->second = false;
				ret = it->first;
				flag = true;
				break;	
			}
		}	
		if(flag == true)
		{
			break;
		}
		else
		{
			usleep(1000);
			continue;
		}
	}
	return ret;
}

// 释放一个连接还给线程池
int MysqlPool::releaseConnection(MysqlObj* conn)
{
	for(auto it = mysql_map.begin(); it != mysql_map.end(); ++it)
	{
		unique_lock<mutex> lk(resource_mutex);
		if(it->first == conn)
		{
			it->second = true;
			break;
		}
	}
	return 1;
}

string MysqlPool::ErrorMessage() const
{
	return m_strErrorMessage; 
}
