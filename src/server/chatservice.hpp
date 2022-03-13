//
// Created by jack on 2022/3/7.
//

#ifndef MYMUDUO_CHATSERVICE_HPP
#define MYMUDUO_CHATSERVICE_HPP

#include <functional>
#include <unordered_map>
#include <mutex>
#include "nlohmann/json.hpp"
#include "muduo/net/TcpConnection.h"
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"

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
    //存储在线的用户链接
    std::unordered_map<int,muduo::net::TcpConnectionPtr> _userConnMap;
    // 定义互斥锁，保证线程安全
    std::mutex _connMutex;
    // 数据库操作类
    UserModel _userModel;
    // 离线消息存储查询类
    OfflineMsgModel _offlinemsgmodel;
    // 好友信息操作类
    FriendModel _friendmodel;
    ChatService();
public:
    static ChatService* instance();
    // 登录业务
    void login(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    //注册业务
    void regiseter(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    // 一对一聊天业务
    void oneChat(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    // 添加好友业务
    void addFriend(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    // 获取对应消息的处理器
    MsgHandler getHandler(int msgid);
    //客户端异常退出
    void clientCloseException(const muduo::net::TcpConnectionPtr &conn);
    // 服务器异常，业务重置方法
    void reset();
};

#endif //MYMUDUO_CHATSERVICE_HPP
