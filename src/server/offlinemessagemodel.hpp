//
// Created by jack on 2022/3/12.
//

#ifndef MYMUDUO_OFFLINEMESSAGEMODEL_HPP
#define MYMUDUO_OFFLINEMESSAGEMODEL_HPP
#include <string>
#include <vector>
// 提供离线消息表的操作接口方法
class OfflineMsgModel
{
public:
    void insert(int id,std::string msg);
    void remove(int id);
    std::vector<std::string > query(int id);
};
#endif //MYMUDUO_OFFLINEMESSAGEMODEL_HPP
