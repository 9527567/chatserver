//
// Created by jack on 2022/3/7.
//
#include "chatserver.hpp"
#include <functional>
#include "nlohmann/json.hpp"
#include "chatservice.hpp"
using json = nlohmann::json;
//启动服务
void ChatServer::start()
{
    _server.start();
}
//构造函数
ChatServer::ChatServer(
        net::EventLoop *loop,
        const net::InetAddress &listenAddr,
        const std::string &nameArg) : _loop(loop), _server(loop, listenAddr, nameArg)
{
    //注册连接回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection,this,std::placeholders::_1));
    //消息回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage,this,std::placeholders::_1,_2,_3));
    //线程数量
    _server.setThreadNum(4);
}

void ChatServer::onConnection(const net::TcpConnectionPtr &conn)
{
    //客户端断开连接
    if(!conn->connected())
    {
        conn->shutdown();
    }
}
// 网络层不调用业务层代码，通过回调和绑定
void ChatServer::onMessage(const net::TcpConnectionPtr &conn, net::Buffer *buffer, Timestamp time)
{
    // buffer缓冲区
    string buf = buffer->retrieveAllAsString();
    // 数据反序列化，完全解耦网络模块和业务模块的代码，通过回调函数的方式
    json js = json::parse(buf);
    auto msgHandler =  ChatService::instance()->getHandler(js["msgid"].get<int>());
    // 回调消息绑定好的事件处理器，来执行相应的任务
    msgHandler(conn,js,time);
}
