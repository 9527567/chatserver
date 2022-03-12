//
// Created by jack on 2022/3/12.
//
#include "offlinemessagemodel.hpp"
#include "db.hpp"

void OfflineMsgModel::insert(int id, std::string msg)
{
    // 1. 组装MySQL语句
    char sql[1024]{0};
    std::sprintf(sql, "insert into offlinemessage(userid,message) value('%d','%s')", id, msg.c_str());
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

void OfflineMsgModel::remove(int id)
{
    // 1. 组装MySQL语句
    char sql[1024]{0};
    std::sprintf(sql, "delete from offlinemessage where userid = %d", id);
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

std::vector<std::string> OfflineMsgModel::query(int id)
{
    std::vector<std::string> vec;
    // 1. 组装MySQL语句
    char sql[1024]{0};
    std::sprintf(sql, "select message from offlinemessage where userid = %d", id);
    MySQL mysql;
    if (mysql.connect())
    {
        auto *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                vec.emplace_back(row[0]);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}