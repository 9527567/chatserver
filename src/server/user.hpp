//
// Created by jack on 2022/3/9.
//

#ifndef MYMUDUO_USER_HPP
#define MYMUDUO_USER_HPP
#include <string>
//user表的orm类，映射user表的数据结构类
class User
{
private:
    int id;
    std::string name;
    std::string password;
    std::string state;
public:
    User(int _id = -1, std::string _name = "", std::string _password = "", std::string _state = "offline") : id(_id),
                                                                                                             name(_name),
                                                                                                             password(
                                                                                                                     _password),
                                                                                                             state(_state)
    {}

    void setId(int id)
    { this->id = id; }

    void setName(std::string name)
    { this->name = name; }

    void setPassword(std::string password)
    { this->password = password; }

    void setState(std::string state)
    { this->state = state; }

    int getId()
    { return this->id; }

    std::string getName()
    { return this->name; }

    std::string getPassword()
    { return this->password; }

    std::string getState()
    { return this->state; }
};

#endif //MYMUDUO_USER_HPP
