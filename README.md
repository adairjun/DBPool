
DBPool
======


a mysqlclient based C++ connection pool

make sure there is mysql-devel, mysql-libs, boost in your system

使用make来生成数据库连接池的静态库libdbpool.a和动态库libdbpool.so
使用make check编译src/目录下的测试文件

Makefile的quiet_cxx有点问题
使用gtest来测试模块
将some改为map
