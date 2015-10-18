#!/bin/bash
#  这个安装依赖库的脚本或许可以直接使用git submodule update --init的方式来安装
# Make sure only root can run our script
function rootness()
{
    if [[ $EUID -ne 0 ]]; then
       echo "Error:This script must be run as root!" 1>&2
       exit 1
    fi
}

function install_gtest()
{
	wget https://codeload.github.com/google/googletest/tar.gz/release-1.7.0	
	tar -zxvf googletest-release-1.7.0.tar.gz
	cd googletest-release-1.7.0/
	g++ -isystem ./include -I. -pthread -c ./src/gtest-all.cc				
	ar -rv libgtest.a gtest-all.o
	cp -r ./include/gtest/ /usr/local/include
	cp ./libgtest.a /usr/local/lib
	cd ..
	rm -rf googletest-release-1.7.0.tar.gz/	
}

function install_hiredis()
{
	wget https://codeload.github.com/redis/hiredis/tar.gz/v0.13.3
	tar -zxvf hiredis-0.13.3.tar.gz 
	cd hiredis-0.13.3/
	make 
	make install	
}

rootness

if [ ! `whereis yum | gawk '{print $2}'` = "" ]
then
	echo -ne "\033[32m"; echo "*************start install mysql and boost********"; echo -ne "\033[0m"
	yum -y install mysql-server
	yum -y install redis
	yum -y install mysql-devel
	yum -y install mysql-libs
	yum -y install boost boost-devel boost-doc
fi

if [ ! `whereis apt-get | gawk '{print $2}'` = "" ]
then
	echo -ne "\033[32m"; echo "*************start install mysql and boost********"; echo -ne "\033[0m"
	apt-get -y install mysql-server
	apt-get -y install redis-server
	apt-get -y install libmysqlclient-dev
	apt-get -y install libmysqld-dev
	apt-get -y install libboost-dev
fi

install_gtest
install_hiredis
