//
// Created by jack on 2022/3/7.
//

#ifndef MYMUDUO_PUBLIC_HPP
#define MYMUDUO_PUBLIC_HPP
// 使用传入json的msgid来定义行为
enum  EnMsgType
{
    LOGIN_MSG = 1,//登录消息
    REG_MSG, //注册消息
    LOGIN_MSG_ACK, // 登录响应消息
    REG_MSG_ACK ,//注册响应消息
    ONE_CHAT_MSG,// 单一聊天消息
};
#endif //MYMUDUO_PUBLIC_HPP
