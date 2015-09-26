#include "../include/redis_obj.h"
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <gtest/gtest.h>

using namespace std;

using std::cout;
using std::endl;

void hello() { }

TEST(helloTest, Test1)
{
	// 从配置文件database.xml当中读入mysql的ip, 用户, 密码, 数据库名称,	
	boost::property_tree::ptree pt;	
	const char* xml_path = "../config/database.xml";	
	boost::property_tree::read_xml(xml_path, pt);
	
	string host_;
	string password_;
	unsigned port_;
	//这段注释的代码是读取json配置文件的
//	const char* json_path = "../config/database.json";
//	boost::property_tree::read_json(json_path, pt);

	BOOST_AUTO(child, pt.get_child("Config.RedisConnection"));
	for(BOOST_AUTO(pos, child.begin()); pos!= child.end(); ++pos)
	{
		if(pos->first == "IP")
			host_ = pos->second.data();
		if(pos->first == "Port")
			port_ = boost::lexical_cast<int>(pos->second.data());
		if(pos->first == "Passwd")
			password_ = pos->second.data();
	}

	RedisObj useRed(host_, password_, port_);	
	EXPECT_TRUE(useRed.Connect());
	EXPECT_EQ(0,useRed.ExecuteCmd("set test hello"));
	EXPECT_EQ(0,useRed.ExecuteCmd("get test"));
	string result;
	EXPECT_EQ(1,useRed.StringResult(result));
	useRed.Close();		
	cout << result << endl;
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
