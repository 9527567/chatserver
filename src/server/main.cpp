//
// Created by jack on 2022/3/7.
//
#include "chatserver.hpp"
#include "chatservice.hpp"
#include <signal.h>
void resetHandler(int)
{
    ChatService::instance()->reset();
    exit(0);
}
int main()
{
    signal(SIGINT,resetHandler);
    net::EventLoop loop;
    net::InetAddress addr("127.0.0.1",6000);
    ChatServer server(&loop,addr,"Chatserver");
    server.start();
    loop.loop();
    return 0;
}

