//
// Created by jack on 2022/3/9.
//

#ifndef MYMUDUO_USERMODEL_HPP
#define MYMUDUO_USERMODEL_HPP
#include "user.hpp"
// user表的数据操作类
class UserModel
{
public:
    // user表的增加方法
    bool insert(User &user);
    // 根据用户id查询用户id
    User query(int id);
    bool updateState(User user);

    void resetState();
};

#endif //MYMUDUO_USERMODEL_HPP
