//
// Created by jack on 2022/3/9.
//
#include "usermodel.hpp"
#include "db.hpp"
#include <iostream>

bool UserModel::insert(User &user)
{
    // 1. 组装MySQL语句，不会sql注入吗
    char sql[1024]{0};
    std::sprintf(sql, "insert into User(name,password,state) value('%s','%s','%s')", user.getName().c_str(),
                 user.getPassword().c_str(), user.getState().c_str());
    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            // 获取插入成功的用户的主键id
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}