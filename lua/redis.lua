#!/usr/bin/lua
-- 导入redis-lua模块-- 
local redis = require("redis")
-- 连接redis --
local client = redis.connect("127.0.0.1", 6379)
local response = client:ping()

local value = client:get("user:book")
print(value)
