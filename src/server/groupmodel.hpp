//
// Created by jack on 2022/3/14.
//

#ifndef MYMUDUO_GROUPMODEL_HPP
#define MYMUDUO_GROUPMODEL_HPP
#include "group.hpp"

class GroupModel
{
public:
    // 创建群组
    bool createGroup(Group &group);
    // 加入群组
    bool addGroup(int userid,int groupid,std::string role);
    // 查询用户所在群组的信息
    std::vector<Group> queryGroups(int userid);
    std::vector<int> queryGroupUsers(int userid,int groupid);

};

#endif //MYMUDUO_GROUPMODEL_HPP
