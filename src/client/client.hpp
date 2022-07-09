//
// Created by jack on 2022/7/7.
//

#ifndef MYMUDUO_CLIENT_HPP
#define MYMUDUO_CLIENT_HPP
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>


// linux 网络编程
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "server/group.hpp"
#include "server/user.hpp"
#include "server/public.hpp"
//记录当前系统登录的用户信息
User g_currentUser;
// 记录当前登录用户的好友列表信息
std::vector<User> g_currentUserFriendList;
// 记录当前登录成功用户的基本信息
std::vector<Group> g_currentUserGroupList;
// 显示当前登录成功用户的基本信息
// 系统支持的客户端命令列表,是命令而不是命令行参数
std::unordered_map<std::string ,std::string > commandMap  {
        {"help","显示所有的命令，格式help"},
        {"chat","一对一聊天，格式：chat:friendid:message"},
        {"addfriend","添加好友，格式addfriend:friendid"},
        {"creategroup","创建群组，格式creategroup:groupname:groupdesc"},
        {"addgroup","加入群组，格式addgroup:groupid"},
        {"groupchat","群聊，格式groupchat：groupid：message"},
        {"loginout","注销，格式loginout"},
};
void help(int = 0,std::string ="");
void chat(int ,std::string);
void addfriend(int ,std::string);
void creategroup(int ,std::string);
void addgroup(int ,std::string);
void groupchat(int ,std::string);
void loginout(int ,std::string);
// 注册系统支持的客户端命令处理
std::unordered_map<std::string,std::function<void(int,std::string)>> commandHandlerMap {
        {"help",help},
        {"chat",chat},
        {"addfriend",addfriend},
        {"creategroup",creategroup},
        {"addgroup",addgroup},
        {"groupchat",groupchat},
        {"loginout",loginout},
};

void showCurrentUserData();
// 接收线程
void readTaskHandler(int clientfd);
// 获取时间
std::string getCurrentTime();
// 主聊天页面程序
void mainMenu(int clientfd);
#endif //MYMUDUO_CLIENT_HPP
