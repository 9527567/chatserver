//
// Created by jack on 2022/3/13.
//
#include "friendmodel.hpp"
#include "db.hpp"
void FriendModel::insert(int id, int friendid)
{
    // 1. 组装MySQL语句
    char sql[1024]{0};
    std::sprintf(sql, "insert into friend value(%d,%d)", id,friendid);
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return;
        }
    }
}

std::vector<User> FriendModel::query(int userid)
{
    std::vector<User> vec;
    // 1. 组装MySQL语句
    char sql[1024]{0};
    std::sprintf(sql, "select a.id,a.name,a.state from user a inner join friend b on b.friendid = a.id where b.userid = %d", userid);
    MySQL mysql;
    if (mysql.connect())
    {
        auto *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[3]);
                vec.push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}