# chatserver

#### 介绍
基于muduo库实现自己的聊天系统

####文件目录
- src 源码
    - muduo muduo网络库，修改少量的CMakeLists.txt直接源码编译
    - nlohmann json解析库
    - server 服务端核心代码
- CMakeLists.txt

#### 编译
安装mariadb，mariadb-client-dev
~~~bash
sudo apt install mariadb-server libmariadb-dev mariadb-client
~~~
设置 `utf-8mb4` 编码 

`vim /etc/my.cnf`
~~~conf
[client]
default-character-set = utf8mb4

[mysql]
default-character-set = utf8mb4

[mysqld]
character-set-client-handshake = FALSE
character-set-server = utf8mb4
collation-server = utf8mb4_unicode_ci
~~~
`sudo systemctl restart mariadb`
~~~bash
sudo mysql
set password for root@localhost = password('qdu321');
flush privileges;
source chat.sql;
quit
~~~
#### 使用
需要启动mariadb服务，创建对应的数据库和表
~~~bash
#安装 redis
sudo apt install redis
/etc/init.d/redis-server start
~~~

