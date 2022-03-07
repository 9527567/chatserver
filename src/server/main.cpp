//
// Created by jack on 2022/3/7.
//
#include "chatserver.hpp"
#include <iostream>

int main()
{
    net::EventLoop loop;
    net::InetAddress addr("127.0.0.1",6000);
    Chatserver server(&loop,addr,"Chatserver");
    return 0;
}

