
DBPool
======

make sure there is mysql-devel, mysql-libs, boost, hiredis, gtest, theron in your system
======

使用make来生成数据库连接池的静态库libdbpool.a和动态库libdbpool.so
======
使用make check编译gtest/目录下的测试文件
======

使用gtest来测试代码,使用theron库来进行C++的高并发,测试是否线程安全
======

内嵌lua来连接mysql,只是为了学习一下lua
可以直接用lua写出连接池,再使用C++来调用lua

要使用lua,首先需要安装luarocks
然后使用
luarocks install luasql-mysql
luarocks install redis-lua
使用luarocks list来查看安装
