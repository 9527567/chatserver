//
// Created by jack on 2022/3/7.
//
#include "chatservice.hpp"
#include "public.hpp"
#include "muduo/base/Logging.h"


// 获取单例对象的指针
ChatService *ChatService::instance()
{
    static ChatService service;
    return &service;
}

//注册回调
ChatService::ChatService()
{
    // 搞清楚这里到底是什么意思。
    _msgHandlerMap.insert({EnMsgType::LOGIN_MSG,
                           std::bind(&ChatService::login, this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3)});
    _msgHandlerMap.insert({EnMsgType::REG_MSG,
                           std::bind(&ChatService::regiseter, this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3)});
}

void ChatService::login(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp)
{

}

void ChatService::regiseter(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp)
{
    
}

MsgHandler ChatService::getHandler(int msgid)
{
    // mdgid没有对应的事件处理回调
    auto it = _msgHandlerMap.find(msgid);
    if(it==_msgHandlerMap.end())
    {

        // 返回默认的处理器
        return [=](const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp)
        {
            LOG_ERROR << "msgid" << msgid << "can not find handler!";
        };
    }
    return _msgHandlerMap[msgid];
}
