#include "../include/pool.h"
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

void Pool::lock(){
	m_mutex.lock();
}

void Pool::unlock(){
	m_mutex.unlock();
}

void Pool::locki()
{
	initmutex.lock();
}

void Pool::unlocki()
{
	initmutex.unlock();
}	

Pool::Pool()
{
	// 从配置文件mysql.xml当中读入mysql的ip, 用户, 密码, 数据库名称,	
	boost::property_tree::ptree pt;	
	const char* xml_path = "../config/mysql.xml";	
	boost::property_tree::read_xml(xml_path, pt);
	
	//这段注释的代码是读取json配置文件的
//	const char* json_path = "../config/mysql.json";
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
			mysql_map.insert(make_pair(conn, true));
		else
		{
			m_strErrorMessage = conn->ErrorMessage();
			delete conn;
		}
	}

}

Pool::~Pool()
{
	// 析构函数就是顺序销毁MysqlObj指针
	
	for(auto it = mysql_map.begin(); it != mysql_map.end(); ++it)
	{
		delete it->first;
	}
	mysql_map.clear();
}


// 从map当中得到一个连接
MysqlObj* Pool::getConnection()
{
	//get connection operation
	MysqlObj* ret = NULL;
	while(true)
	{
		Pool::lock();
		bool flag = false;
		for(auto it = mysql_map.begin(); it != mysql_map.end(); ++it)
		{
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
			Pool::unlock();
			break;
		}
		else
		{
			//cout << "wait" << endl;
			Pool::unlock();
			usleep(1000);
			continue;
		}
	}
	return ret;
}

// 释放一个连接还给线程池
int Pool::releaseConnection(MysqlObj* conn)
{
	lock();
	for(auto it = mysql_map.begin(); it != mysql_map.end(); ++it)
	{
		if(it->first == conn)
		{
			it->second = true;
			break;
		}
	}
	unlock();
	return 1;
}

string Pool::ErrorMessage() const
{
	return m_strErrorMessage; 
}
