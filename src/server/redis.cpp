//
// Created by jack on 2022/7/17.
//
#include "redis.hpp"
#include <iostream>
#include <utility>

Redis::Redis() : _publish_context(nullptr), _subscribe_context(nullptr)
{}

Redis::~Redis()
{
    if (_publish_context != nullptr)
    {
        redisFree(_publish_context);
    }
    if (_subscribe_context != nullptr)
    {
        redisFree(_subscribe_context);
    }
}

bool Redis::connect()
{
    _publish_context = redisConnect("127.0.0.1", 6379);
    if (nullptr == _publish_context)
    {
        std::cerr << "connect redis failed" << std::endl;
        return false;
    }
    _subscribe_context = redisConnect("127.0.0.1", 6379);
    if (nullptr == _subscribe_context)
    {
        std::cerr << "connect redis failed" << std::endl;
        return false;
    }
    // 在单独的线程中监听通道上的事件，有消息给业务层进行上报
    std::thread t([&]()
                  {
                      observer_channel_message();
                  });
    t.detach();
    return true;
}

bool Redis::publish(int channel, const std::string &message)
{
    redisReply *reply = (redisReply *) redisCommand(_publish_context, "PUBLISH %d %s", channel, message.c_str());
    if (nullptr == reply)
    {
        std::cerr << "publish command failed!" << std::endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}

// 向redis指定的通道subscribe订阅消息
bool Redis::subscribe(int channel)
{
    if (REDIS_ERR == redisAppendCommand(this->_subscribe_context, "SUBSCRIBE %d", channel))
    {
        std::cerr << "subscribe command failed!" << std::endl;
        return false;
    }
    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(this->_subscribe_context, &done))
        {
            std::cerr << "subscribe command failed!" << std::endl;
            return false;
        }
    }
    return true;
}

bool Redis::unsubscribe(int channel)
{
    if (REDIS_ERR == redisAppendCommand(this->_subscribe_context, "UNSUBSCRIBE %d", channel))
    {
        std::cerr << "subscribe command failed!" << std::endl;
        return false;
    }
    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(this->_subscribe_context, &done))
        {
            std::cerr << "subscribe command failed!" << std::endl;
            return false;
        }
    }
    return true;
}

void Redis::observer_channel_message()
{
    redisReply *reply = nullptr;
    while(REDIS_OK == redisGetReply(this->_subscribe_context,(void **)&reply))
    {
        // 订阅收到的消息使一个带三元素的数据
        if (reply!= nullptr&&reply->element[2]!= nullptr&&reply->element[2]->str!= nullptr)
        {
            // 给业务层上报通道的消息
            _notify_message_handler(atoi(reply->element[1]->str),reply->element[2]->str);
        }
        freeReplyObject(reply);
    }
    std::cerr << ">>>>>>>>>>>>>>>>>>>> observer_channel_message quit <<<<<<<<<<<<<<<<<<<<" << std::endl;
}

void Redis::init_notify_handler(std::function<void(int, std::string)> fn)
{
    this->_notify_message_handler = std::move(fn);
}
