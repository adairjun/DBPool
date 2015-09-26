#!/bin/bash

# Make sure only root can run our script
function rootness(){
    if [[ $EUID -ne 0 ]]; then
       echo "Error:This script must be run as root!" 1>&2
       exit 1
    fi
}

if [ ! `whereis yum | gawk '{print $2}'` = "" ]
then
	echo -ne "\033[32m"; echo "*************start install mysql and boost********"; echo -ne "\033[0m"
	yum -y install mysql-devel
	yum -y install mysql-libs
	yum -y install boost boost-devel boost-doc
fi

if [ ! `whereis apt-get | gawk '{print $2}'` = "" ]
then
	echo -ne "\033[32m"; echo "*************start install mysql and boost********"; echo -ne "\033[0m"
	apt-get -y install libmysqlclient-dev
	apt-get -y install libmysqld-dev
	apt-get -y install libboost-dev
	apt-get -y install redis-server
fi
