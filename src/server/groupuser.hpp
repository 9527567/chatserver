//
// Created by jack on 2022/3/13.
//

#ifndef MYMUDUO_GROUPUSER_HPP
#define MYMUDUO_GROUPUSER_HPP

#include "user.hpp"
#include <string>

class GroupUser : public User
{
public:
    void setRole(std::string role)
    { this->role = role; }

    std::string getRole()
    { return this->role; }

private:
    std::string role;
};

#endif //MYMUDUO_GROUPUSER_HPP
