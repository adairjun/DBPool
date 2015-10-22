
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
