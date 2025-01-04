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
#include "groupmodel.hpp"
#include "redis.hpp"

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
    GroupModel _groupModel;
    Redis _redis;
    ChatService();
public:
    static ChatService* instance();
    // 登录业务
    void login(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    // 注销业务
    void loginout(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    //注册业务
    void regiseter(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    // 一对一聊天业务
    void oneChat(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    //查找好友
    void searchFriend(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    // 添加好友请求
    void addFriendRequest(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    // 添加好友响应
    void addFriendResponse(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    // 获取好友列表
    void listFriend(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    // 获取对应消息的处理器
    MsgHandler getHandler(int msgid);
    //客户端异常退出
    void clientCloseException(const muduo::net::TcpConnectionPtr &conn);
    // 创建群组
    void createGroup(const muduo::net::TcpConnectionPtr &conn,json &js,muduo::Timestamp time);
    // 查找群组
    void searchGroup(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    // 加入群组请求
    void addGroupResquest(const muduo::net::TcpConnectionPtr &conn,json &js,muduo::Timestamp time);
    // 加入群组响应
    void addGroupResponse(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    // 获取群组列表
    void listGroup(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    // 获取群组成员
    void listGroupMember(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    // 添加群组成员
    void addGroupMember(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    // 删除群组成员
    void delGroupMember(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);
    // 群组聊天
    void groupChat(const muduo::net::TcpConnectionPtr &conn,json &js,muduo::Timestamp time);
    // 服务器异常，业务重置方法
    void reset();
    // 从redis获取订阅消息
    void handleRedisSubscribeMessage(int, const std::string&);
    // 设置头像
    void setIcon(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time);

};  

#endif //MYMUDUO_CHATSERVICE_HPP
