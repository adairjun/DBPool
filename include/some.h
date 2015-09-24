#ifndef _SOME_H
#define _SOME_H

#include <boost/shared_ptr.hpp>
#include "mysql_obj.h"

// some类的作用就是map,提供一个从Connection*到bool的映射,这个后期将改成map来实现
class some
{
 public:
//	boost::shared_ptr<MysqlObj> first;
	MysqlObj* first;
	bool second;
};
#endif /* SOME_H */
