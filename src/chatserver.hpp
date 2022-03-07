//
// Created by jack on 2022/3/6.
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
    net::EventLoop *_loop;

public:

};
#endif //MYMUDUO_CHATSERVER_HPP
