//
// Created by jack on 2022/7/17.
//
#include "hiredis.h"
#include <thread>
#include <functional>
class Redis
{
public:
    Redis();
    ~Redis();
    // 链接redis服务器
    bool connect();
    // 向redis指定的通道channel发布消息
    bool publish(int,const std::string&);
    // 向redis指定的通道channel订阅消息
    bool subscribe(int);
    // 取消订阅
    bool unsubscribe(int);
    // 在独立线程中接受订阅通道中的消息
    void observer_channel_message();
    // 初始化像业务层上报通道消息的回调对象
    void init_notify_handler(std::function<void(int,std::string)> fn);
private:
    // redis同步上下文对象，负责publish消息
    redisContext *_publish_context;
    // redis同步上下文对象，负责subscribe消息(接收)
    redisContext *_subscribe_context;
    // 回调函数，收到订阅消息，给service上报
    std::function<void(int,std::string)> _notify_message_handler;
};