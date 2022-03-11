//
// Created by jack on 2022/3/7.
//

#ifndef MYMUDUO_CHATSERVICE_HPP
#define MYMUDUO_CHATSERVICE_HPP

#include <functional>
#include <unordered_map>
#include "nlohmann/json.hpp"
#include "muduo/net/TcpConnection.h"
#include "usermodel.hpp"
using json = nlohmann::json;
//表示处理消息的事件回调方法类型
using MsgHandler = std::function<void(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp)>;
// 单例模式
// 聊天服务器业务类代码
class ChatService
{
private:
    //存储消息id和其对应事件的业务处理方法
    std::unordered_map<int,MsgHandler> _msgHandlerMap;
    UserModel _userModel;
    ChatService();
public:
    static ChatService* instance();
    void login(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp);

    void regiseter(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp);
    // 获取对应消息的处理器
    MsgHandler getHandler(int msgid);

};

#endif //MYMUDUO_CHATSERVICE_HPP
