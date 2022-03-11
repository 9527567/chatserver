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
安装mysql，mysql-client-dev
~~~bash
sudo apt install mysql-server libmysqlclient-dev
~~~
#### 使用
需要启动mysql服务，创建对应的数据库和表
