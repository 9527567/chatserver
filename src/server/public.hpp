//
// Created by jack on 2022/3/7.
//

#ifndef MYMUDUO_PUBLIC_HPP
#define MYMUDUO_PUBLIC_HPP
// 使用传入json的msgid来定义行为
//使用更先进的枚举类语法
enum class EnMsgType
{
    LOGIN_MSG = 1,//登录消息
    LOGIN_OUT_MSG,// 注销消息
    REG_MSG, //注册消息
    LOGIN_MSG_ACK, // 登录响应消息
    REG_MSG_ACK ,//注册响应消息
    ONE_CHAT_MSG,// 单一聊天消息
    ADD_FRIEND_MSG,//添加好友消息

    CREATE_GROUP_MSG,//创建聊天组
    ADD_GROUP_MSG, // 加入聊天组
    GROUP_CHAT_MSG,// 组聊天
};
#endif //MYMUDUO_PUBLIC_HPP
