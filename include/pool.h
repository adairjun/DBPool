
/* testsql.c
 ** An example to use MYSQL C API
 ** Copyright 2004 Coon Xu.
 ** Author: Coon Xu
 ** Date: 05 Nov 2004
 */

#include <string.h>
#include <mutex>
#include "some.h"

using std::string;
using std::vector;
using std::mutex;

// 其实Connection才是连接mysql的,Connection相当于wiwo的MysqlObject,这个pool只做了一件事情,那就是创建poolsize个的连接,Mysql的最大连接数目是151,销毁pool就是,
class Pool{
 public:
	// 批量创建连接在构造函数当中进行
	Pool();
	// 析构函数就是顺序销毁Connection指针
	// 最后销毁vec
	virtual ~Pool();
		
	// 先调用构造函数批量创建链接,构造函数会产生一个Pool的指针,然后从数组当中选取一个连接,造成了极大的浪费
	MysqlObj* getConnection();
	
	// 释放特定的连接就是把数组当中的bool值置为false
	int releaseConnection(MysqlObj*);
	void lock();
	void unlock();

	void locki();
	void unlocki();
	
 private:
	mutex m_mutex;
	mutex initmutex;

 private:
	some* vec;     // need an instance to init

	// 之所以要把这里设置成为public,是因为使用Pool::host_的方式来访问
 public:
	string host_ ;
	string user_ ;
	string password_ ;
	string dbname_;
	unsigned port_;
	
	//Mysql的最大连接数为151,这里建立150个连接
	static const int poolSize_ = 150;

};

