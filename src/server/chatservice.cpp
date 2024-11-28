//
// Created by jack on 2022/3/7.
//
#include "chatservice.hpp"
#include "public.hpp"
#include "muduo/base/Logging.h"
#include <vector>
#include <iostream>
#include "ChaoticStreamCipher.hpp"

// 获取单例对象的指针
ChatService *ChatService::instance()
{
    static ChatService service;
    return &service;
}

//注册回调
ChatService::ChatService()
{
    // 搞清楚这里到底是什么意思。this指向的谁？调用该成员函数对象的地址
    // 在执行任何成员函数时，该成员函数都会自动包含一个隐藏的指针，称为this指针。
    _msgHandlerMap.insert({static_cast<int>(EnMsgType::LOGIN_MSG),
                           std::bind(&ChatService::login, this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3)});
    _msgHandlerMap.insert({static_cast<int>(EnMsgType::LOGIN_OUT_MSG),
                           std::bind(&ChatService::loginout, this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3)});
    _msgHandlerMap.insert({static_cast<int>(EnMsgType::REG_MSG),
                           std::bind(&ChatService::regiseter, this, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3)});
    _msgHandlerMap.insert(std::pair(static_cast<int>(EnMsgType::ONE_CHAT_MSG),
                                    std::bind(&ChatService::oneChat, this, std::placeholders::_1, std::placeholders::_2,
                                              std::placeholders::_3)));
    _msgHandlerMap.insert(std::pair(static_cast<int>(EnMsgType::ADD_FRIEND_MSG),
                                    std::bind(&ChatService::addFriend, this, std::placeholders::_1,
                                              std::placeholders::_2,
                                              std::placeholders::_3)));
    _msgHandlerMap.insert(std::pair(static_cast<int>(EnMsgType::CREATE_GROUP_MSG),
                                    std::bind(&ChatService::createGroup, this, std::placeholders::_1,
                                              std::placeholders::_2,
                                              std::placeholders::_3)));
    _msgHandlerMap.insert(std::pair(static_cast<int>(EnMsgType::ADD_GROUP_MSG),
                                    std::bind(&ChatService::addGroup, this, std::placeholders::_1,
                                              std::placeholders::_2,
                                              std::placeholders::_3)));
    _msgHandlerMap.insert(std::pair(static_cast<int>(EnMsgType::GROUP_CHAT_MSG),
                                    std::bind(&ChatService::groupChat, this, std::placeholders::_1,
                                              std::placeholders::_2,
                                              std::placeholders::_3)));
    if (_redis.connect())
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, std::placeholders::_1,
                                             std::placeholders::_2));
}


MsgHandler ChatService::getHandler(int msgid)
{
    // mdgid没有对应的事件处理回调
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {

        // 返回默认的处理器
        return [=](const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
        {
            LOG_ERROR << "msgid" << msgid << "can not find handler!";
        };
    }
    return _msgHandlerMap[msgid];
}

void ChatService::login(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    ChaoticStreamCipher cipher{0.3,0.4};
    int id = js["id"];
    std::string pwd = js["password"];
    User user = _userModel.query(id);
    if (user.getId() == id && user.getPassword() == pwd)
    {

        if (user.getState() == "online")
        {
            // 该用户已经登陆，不允许登录
            json response;
            response["msgid"] = EnMsgType::LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "该账户已登录";
            conn->send(cipher.encrypt(response.dump()));
        } else
        {
            //登录成功，记录用户信息，stl标准库没有考虑线程安全问题,使用mutex对map加锁
            {
                std::lock_guard<std::mutex> lock(_connMutex);
                _userConnMap.insert(std::pair<int, muduo::net::TcpConnectionPtr>(user.getId(), conn));
            }
            // 登录成功，向redis订阅channel(id)
            _redis.subscribe(id);
            //登录成功，更新用户状态
            user.setState("online");
            _userModel.updateState(user);
            json response;
            response["msgid"] = EnMsgType::LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
            // 查询离线消息
            std::vector<std::string> vec = _offlinemsgmodel.query(user.getId());
            if (!vec.empty())
            {
                response["offlinemsg"] = vec;
                _offlinemsgmodel.remove(user.getId());
            }
            // 查询好友信息并返回
            std::vector<User> userVec = _friendmodel.query(id);
            if (!userVec.empty())
            {
                std::vector<std::string> vec2;
                json js;
                for (auto &i: userVec)
                {
                    js["id"] = i.getId();
                    js["name"] = i.getName();
                    js["state"] = i.getState();
                    vec2.push_back(js.dump());
                }
                response["friends"] = vec2;
            }
            // 查询用户群组消息
            std::vector<Group> groupuserVec = _groupModel.queryGroups(id);
            if (!groupuserVec.empty())
            {
                // group:[{groupid:[xxx,xxx,xxx]}]
                std::vector<std::string> groupV;
                for (auto &group: groupuserVec)
                {
                    json grpjson;
                    grpjson["id"] = group.getId();
                    grpjson["groupname"] = group.getName();
                    grpjson["groupdesc"] = group.getDesc();
                    std::vector<std::string> userV;
                    for (auto &user: group.getUser())
                    {
                        json js;
                        js["id"] = user.getId();
                        js["name"] = user.getName();
                        js["state"] = user.getState();
                        js["role"] = user.getRole();
                        userV.push_back(js.dump());
                    }
                    grpjson["users"] = userV;
                    groupV.push_back(grpjson.dump());
                }
            }
            conn->send(cipher.encrypt(response.dump()));
            std::cout << "+++" << response.dump() << "+++"<< std::endl;

        }
    } else
    {
        json response;
        response["msgid"] = EnMsgType::LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "用户名或者密码错误";
        conn->send(cipher.encrypt(response.dump()));
    }
}

void ChatService::regiseter(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    ChaoticStreamCipher cipher{0.3, 0.4};
    std::string name = js["name"];
    std::string pwd = js["password"];
    User user;
    user.setName(name);
    user.setPassword(pwd);
    bool state = _userModel.insert(user);
    if (state)
    {
        json response;
        response["msgid"] = EnMsgType::REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(cipher.encrypt(response.dump()));
    } else
    {
        json response;
        response["msgid"] = EnMsgType::REG_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = 1;
        conn->send(cipher.encrypt(response.dump()));
    }
}

void ChatService::loginout(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    int userid = js["id"].get<int>();
    {
        std::lock_guard<std::mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end())
        {
            _userConnMap.erase(it);
        }
    }
    // 更新用户状态信息
    _redis.unsubscribe(userid);
    User user{userid, "", "", "offline"};
    _userModel.updateState(user);
}

void ChatService::clientCloseException(const muduo::net::TcpConnectionPtr &conn)
{
    User user;
    // 因为有多个用户同时登录，所以要对map加锁防止出现线程不安全的情况
    {
        std::lock_guard<std::mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); it++)
        {
            if (it->second == conn)
            {
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }
    _redis.unsubscribe(user.getId());
    if (user.getId() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
}

void ChatService::oneChat(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    ChaoticStreamCipher cipher{0.3, 0.4};
    int toid = js["toid"].get<int>();
    // 在同一台主机登录
    {
        std::lock_guard<std::mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end())
        {
            // 在线
            it->second->send(cipher.encrypt(js.dump()));
            return;
        }
    }
    // 查询是否在其他主机上登录
    User user = _userModel.query(toid);
    if (user.getState() == "online")
    {
        _redis.publish(toid, cipher.encrypt(js.dump()));
        return;
    }
    // 离线消息
    _offlinemsgmodel.insert(toid, js.dump());
}

// 服务端异常退出，重置用户状态
void ChatService::reset()
{
    _userModel.resetState();
}

// 添加好友业务，msgid：业务类型，friendid ，id
void ChatService::addFriend(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    int id = js["id"];
    int friendId = js["friendid"];
    _friendmodel.insert(id, friendId);
    // 也可以添加响应
}

void ChatService::createGroup(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    int userid = js["id"].get<int>();
    std::string name = js["groupname"];
    std::string desc = js["groupdesc"];
    Group group(-1, name, desc);
    if (_groupModel.createGroup(group))
    {
        _groupModel.addGroup(userid, group.getId(), "creator");
    }
}

void ChatService::addGroup(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupModel.addGroup(userid, groupid, "normal");
}

void ChatService::groupChat(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time)
{
    ChaoticStreamCipher cipher{0.3, 0.4};
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    std::vector<int> useridVec = _groupModel.queryGroupUsers(userid, groupid);
    std::lock_guard<std::mutex> lock(_connMutex);
    for (int id: useridVec)
    {
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end())
        {
            it->second->send(cipher.encrypt(js.dump()));
        } else
        {
            // 查询toid是否在线
            User user = _userModel.query(id);
            if (user.getState() == "online")
            {
                _redis.publish(id, cipher.encrypt(js.dump()));
            } else
            {
                // 存储离线消息
                _offlinemsgmodel.insert(id, cipher.encrypt(js.dump()));
            }
        }
    }
}

void ChatService::handleRedisSubscribeMessage(int userid, const std::string &msg)
{
    ChaoticStreamCipher cipher{0.3, 0.4};
    std::lock_guard<std::mutex> lock(_connMutex);
    auto it = _userConnMap.find(userid);
    if (it != _userConnMap.end())
    {
        it->second->send(cipher.encrypt(msg));
        return;
    }

    // 存储该用户的离线消息
    _offlinemsgmodel.insert(userid, msg);
}


