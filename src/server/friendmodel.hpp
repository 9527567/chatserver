//
// Created by jack on 2022/3/12.
//

#ifndef MYMUDUO_FRIENDMODEL_HPP
#define MYMUDUO_FRIENDMODEL_HPP
#include <vector>
#include "user.hpp"
// 维护好友信息的操作接口方法
class FriendModel
{
public:
    // 添加好友关系
    static  void insert(int id,int friendid);
    // 返回用户好友列表
    static std::vector<User> query(int userid);


};
#endif //MYMUDUO_FRIENDMODEL_HPP
