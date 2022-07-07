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
void showCurrentUserData();
// 接收线程
void readTaskHandler(int clientfd);
// 获取时间
void getCurrentTime();
// 主聊天页面程序
void mainMenu();
#endif //MYMUDUO_CLIENT_HPP
