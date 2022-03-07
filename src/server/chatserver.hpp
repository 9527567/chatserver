//
// Created by jack on 2022/3/7.
//

#ifndef MYMUDUO_CHATSERVER_HPP
#define MYMUDUO_CHATSERVER_HPP
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace muduo;

class Chatserver
{
private:
    net::TcpServer _server;
    net::EventLoop* _loop;
    void onConnection(const net::TcpConnectionPtr&);
    void onMessage(
            const net::TcpConnectionPtr&,
            net::Buffer*,
            Timestamp
            );
public:
    Chatserver(net::EventLoop* loop,
               const net::InetAddress &listenAddr,
               const std::string &nameArg
            );
    void start();
};
#endif //MYMUDUO_CHATSERVER_HPP
