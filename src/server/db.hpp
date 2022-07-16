//
// Created by jack on 2022/3/8.
//

#ifndef MYMUDUO_MYSQL_HPP
#define MYMUDUO_MYSQL_HPP

#include <mariadb/mysql.h>
#include "muduo/base/Logging.h"
#include <string>

// 数据库配置信息
static std::string server = "172.18.128.1";
static std::string user = "root";
static std::string password = "qdu321";
static std::string dbname = "chat";

// 数据库操作类
class MySQL
{
public:
// 初始化数据库连接
    MySQL()
    {
        _conn = mysql_init(nullptr);
    }
// 释放数据库连接资源

    ~MySQL()
    {
        if (_conn != nullptr)
            mysql_close(_conn);
    }

// 连接数据库
    bool connect()
    {
        MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(),
                                      password.c_str(), dbname.c_str(), 3306, nullptr, 0);
        if (p != nullptr)
        {
            //从MySQL拉下来的数据进行转换编码，显示中文。
            mysql_query(_conn, "set names gbk");
            LOG_INFO << "MySQL connect success!";
        }
        else
        {
            LOG_INFO << "MySQL connect failed!";
        }
        return p;
    }

// 更新操作
    bool update(std::string sql)
    {
        if (mysql_query(_conn, sql.c_str()))
        {
            LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                     << sql << "更新失败!";
            return false;
        }
        return true;
    }

// 查询操作
    MYSQL_RES *query(std::string sql)
    {
        if (mysql_query(_conn, sql.c_str()))
        {
            LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                     << sql << "查询失败!";
            return nullptr;
        }
        return mysql_use_result(_conn);
    }
    // 返回MySQL链接
    auto* getConnection()
    {
        return _conn;
    }
private:
    MYSQL *_conn;
};

#endif //MYMUDUO_MYSQL_HPP
