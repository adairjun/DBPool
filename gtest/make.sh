#!/bin/bash
# 尝试使用C++的theron库来启动多线程
g++ -std=c++11 -I/home/xiongjun/Desktop/DBPool/include -DNDEBUG -o theron_mysql theron_mysql.cpp -L/home/xiongjun/Desktop/DBPool -ltheron -lgtest -ldbpool -lpthread 
