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
    // 搞清楚这里到底是什么意思。this指向的谁？
    // 在执行任何成员函数时，该成员函数都会自动包含一个隐藏的指针，称为this指针。
    _msgHandlerMap.insert({EnMsgType::LOGIN_MSG,
                           std::bind(&ChatService::login, this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3)});
    _msgHandlerMap.insert({EnMsgType::REG_MSG,
                           std::bind(&ChatService::regiseter, this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3)});
}


MsgHandler ChatService::getHandler(int msgid)
{
    // mdgid没有对应的事件处理回调
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {

        // 返回默认的处理器
        return [=](const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp)
        {
            LOG_ERROR << "msgid" << msgid << "can not find handler!";
        };
    }
    return _msgHandlerMap[msgid];
}

void ChatService::login(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp)
{
    int id = js["id"];
    std::string pwd = js["password"];
    User user = _userModel.query(id);
    if (user.getId() != id && user.getPassword() == pwd)
    {
        if (user.getState() == "online")
        {
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "该账户已登录";
            conn->send(response.dump());
        } else
        {
            //登录成功，记录用户信息，stl标准库没有考虑线程安全问题,使用mutex对map加锁
            {
                std::lock_guard<std::mutex> lock(_connMutex);
                _userConnMap.insert(std::pair<int, muduo::net::TcpConnectionPtr>(user.getId(), conn));
            }

            //登录成功，更新用户状态
            user.setState("online");
            _userModel.updateState(user);
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
            conn->send(response.dump());
        }
    } else
    {
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "用户名或者密码错误";
        conn->send(response.dump());
    }
}

void ChatService::regiseter(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp)
{
    std::string name = js["name"];
    std::string pwd = js["password"];
    User user;
    user.setName(name);
    user.setPassword(pwd);
    bool state = _userModel.insert(user);
    if (state)
    {
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    } else
    {
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = 1;
        conn->send(response.dump());
    }
}

void ChatService::clientCloseException(const muduo::net::TcpConnectionPtr &conn)
{
    User user;
    {
        std::lock_guard<std::mutex> lock(_connMutex);
        for (auto &[k, v]: _userConnMap)
        {
            if (v == conn)
            {
                user.setId(k);
                _userConnMap.erase(k);
                break;
            }
        }
    }
    if (user.getId() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
}