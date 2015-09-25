
DBPool
======


a mysqlclient based C++ connection pool

make sure there is mysql-devel, mysql-libs, boost in your system

使用make来生成数据库连接池的静态库libdbpool.a和动态库libdbpool.so
使用make check编译src/目录下的测试文件
但是在UBUNTU下make check有点问题

Makefile的quiet_cxx有点问题
使用gtest来测试模块

要加入redis数据库连接池,需要使用hiredis,需要在github上下载
