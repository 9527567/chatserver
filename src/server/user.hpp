//
// Created by jack on 2022/3/9.
//

#ifndef MYMUDUO_USER_HPP
#define MYMUDUO_USER_HPP

#include <string>
#include <utility>

// user表的orm类，映射user表的数据结构类
class User {
private:
  int id;
  std::string name;
  std::string password;
  // 0表示登录，1表示未登录
  int state;
  std::string sig;
  std::string icon;

public:
  explicit User(int _id = -1, std::string _name = "",
                std::string _password = "", int _state = 0,
                std::string _sig = "", std::string _icon = "")
      : id(_id), name(_name), password(_password), state(_state), sig(_sig),
        icon(_icon) {}

  void setId(int id) { this->id = id; }

  void setName(std::string name) { this->name = std::move(name); }

  void setPassword(std::string password) {
    this->password = std::move(password);
  }

  void setState(int state) { this->state = std::move(state); }

  [[nodiscard]] int getId() const { return this->id; }

  [[nodiscard]] std::string getName() const { return this->name; }

  [[nodiscard]] std::string getPassword() const { return this->password; }

  [[nodiscard]] std::string getState() const { return this->state; }
};

#endif // MYMUDUO_USER_HPP
