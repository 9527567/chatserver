//
// Created by jack on 2022/3/7.
//
#include "chatserver.hpp"
#include "chatservice.hpp"
#include <iostream>
#include <signal.h>
void resetHandler(int)
{
    ChatService::instance()->reset();
    exit(0);
}
int main(int argc,char** argv)
{
    if(argc < 3)
    {
        std::cerr << "command invaild !example: ./chatServer 127.0.0.1 6000 " << std::endl;
    }
    signal(SIGINT,resetHandler);
    net::EventLoop loop;
    const char *ip = argv[1];
    const uint16_t port = atoi(argv[2]);
    net::InetAddress addr(ip,port);
    ChatServer server(&loop,addr,"Chatserver");
    server.start();
    loop.loop();
    return 0;
}

