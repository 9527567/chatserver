//
// Created by jack on 2022/3/7.
//
#include "chatserver.hpp"
#include <functional>
void Chatserver::start()
{
    _server.start();
}

Chatserver::Chatserver(
        net::EventLoop *loop,
        const net::InetAddress &listenAddr,
        const std::string &nameArg) : _loop(loop), _server(loop, listenAddr, nameArg)
{
    //注册连接回调
    _server.setConnectionCallback(std::bind(&Chatserver::onConnection,this,std::placeholders::_1));
    _server.setMessageCallback(std::bind(&Chatserver::onMessage,this,std::placeholders::_1,_2,_3));
    _server.setThreadNum(4);
}

void Chatserver::onConnection(const net::TcpConnectionPtr &)
{}
void Chatserver::onMessage(const net::TcpConnectionPtr &, net::Buffer *, Timestamp)
{}
