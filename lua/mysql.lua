#!/usr/bin/lua
-- 首先是由luarocks下载的luasql.mysql模块--
local mysql = require("luasql.mysql")
-- 先初始化mysql环境---
local env = assert(mysql.mysql())
-- 连接上数据库 --
local con = assert(env:connect("testdb", "root", "passwd"))
-- 执行SQL语句 --
local cur = assert(con:execute("SHOW DATABASES;"))
-- 或者 local cur = assert(con:execute([[SHOW DATABASES;]])) --
local row = cur:fetch({}, "a")

while row do
  print(row.Database)
  row = cur:fetch(row, "a")
end

cur:close()
con:close()
env:close()
