#include "../include/pool.h"
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

// for DEBUG
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
	const char* xml_path = "../config/mysql.xml";	
	boost::property_tree::ptree pt;	
	boost::property_tree::read_xml(xml_path, pt);

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
	}
	// 构造函数的作用就是根据poolSize的大小来构造多个映射,some其实就是起到了map的作用
	// 每个映射的连接都是同样的host,user,pass,dbname
	vec = new some[poolSize_];

	for(int i=0; i<poolSize_; ++i){
//		boost::shared_ptr<Connection> conn =  
		
		MysqlObj* conn = new MysqlObj(host_, user_, password_, dbname_, port_);
		conn->Connect();
		//std::cout << conn << std::endl;
		//std::cout << host << " " << user << " " << pass << " " << dbname << " " << poolSize << std::endl;
		if(!conn){
			cout << "xPool: new Connection Operation failed" << endl;
			exit(-1);
		}
		vec[i].first = conn;
		vec[i].second = false;
	}

}

Pool::~Pool(){
	// 析构函数就是顺序销毁Connection指针
	// 最后销毁vec
	// 如果把some改成map,把Connection* 该成boost::shared_ptr,可以简化操作
	
	for(int i=0;i<poolSize_;i++){
		delete vec[i].first;
	}
	delete[] vec;
	//mysql_library_end();
}


	// 因为这里是用Connection对象来保存一个mysql连接,pool连接池就是一口气创建最大的连接数目,当需要mysql连接的时候就从pool当中取出来
MysqlObj* Pool::getConnection(){

	//get connection operation
	MysqlObj* ret = NULL;
	while(true)
	{
		Pool::lock();
		bool flag = false;
		for(int i=0;i<poolSize_;i++)
		{
			if(vec[i].second == false)
			{
				vec[i].second = true;
				ret = vec[i].first;
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

int Pool::releaseConnection(MysqlObj* conn){
	lock();
	for(int i=0;i<poolSize_;i++){
		if(vec[i].first == conn ){
			vec[i].second = false;
			break;
		}
	}
	unlock();
	return 1;
}

