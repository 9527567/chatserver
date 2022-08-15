#include "client.hpp"

//client

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cerr << "command invalid! example ./chatClient 127.0.0.1 6000" << std::endl;
        exit(-1);
    }
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);
    // 创建client端的socket
    int clinetfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == clinetfd)
    {
        std::cerr << "socket create err" << std::endl;
        exit(-1);
    }
    // 填写client需要连接的server信息ip+port
    sockaddr_in server;
    memset(&server, 0, sizeof(sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);
    // client与server进行链接
    if (-1 == connect(clinetfd, (sockaddr *) &server, sizeof(sockaddr_in)))
    {
        std::cerr << "connect server error" << std::endl;
        close(clinetfd);
        exit(-1);
    }
    // 初始化读写线程通信的信号量
    sem_init(&rwsem, 0, 0);
    // 链接服务器成功，启动子线程
    std::thread readTask(readTaskHandler, clinetfd);
    readTask.detach();
    //main 线程用于接收用户输入，负责发送数据
    for (;;)
    {
        std::cout << "=========================" << std::endl;
        std::cout << "1.login" << std::endl;
        std::cout << "2.register" << std::endl;
        std::cout << "3.quit" << std::endl;
        std::cout << "=========================" << std::endl;
        std::cout << "choise:";
        int choice = 0;
        std::cin >> choice;
        std::cin.get();// 读取掉缓冲区残留的回车
        switch (choice)
        {
            // login业务
            case 1:
            {
                int id = 0;
                char pwd[50]{0};
                std::cout << "userid:";
                std::cin >> id;
                std::cin.get();
                std::cout << "user-password:";
                std::cin.getline(pwd, 50);// 可以保留空格
                json js;
                js["msgid"] = static_cast<int>(EnMsgType::LOGIN_MSG);
                js["id"] = id;
                js["password"] = pwd;
                std::string request = js.dump();

                g_isLoginSuccess = false;
                int len = send(clinetfd, request.c_str(), strlen(request.c_str()) + 1, 0);
                if (len == -1)
                {
                    std::cerr << "send login msg error:" << request << std::endl;
                }
                sem_wait(&rwsem);
                if (g_isLoginSuccess)
                {
                    // 进入聊天主界面
                    isMainMenuRunning = true;
                    mainMenu(clinetfd);
                }
            }

                break;
            case 2:
            {
                char name[50] = {0};
                char pwd[50]{0};
                std::cout << "username:";
                std::cin.getline(name, 50);
                std::cout << "userpassword:";
                std::cin.getline(pwd, 50);

                json js;
                js["msgid"] = static_cast<int>(EnMsgType::REG_MSG);
                js["name"] = name;
                js["password"] = pwd;
                std::string request = js.dump();
                int len = send(clinetfd, request.c_str(), strlen(request.c_str()) + 1, 0);
                if (len == -1)
                {
                    std::cerr << "send reg msg error" << request << std::endl;
                }
                sem_wait(&rwsem); // 等待信号量，由子线程处理完登录的响应消息后，通知这里
            }
                break;
            case 3:
            {
                close(clinetfd);
                sem_destroy(&rwsem);
                exit(0);
            }
            default:
            {
                std::cerr << "Invalid input!" << std::endl;
                break;
            }
        }
    }

}

void showCurrentUserData()
{
    std::cout << "======================login user======================" << std::endl;
    std::cout << "current login user => id:" << g_currentUser.getId() << " name:" << g_currentUser.getName()
              << std::endl;
    std::cout << "----------------------friend list---------------------" << std::endl;
    if (!g_currentUserFriendList.empty())
    {
        for (User &user: g_currentUserFriendList)
        {
            std::cout << user.getId() << " " << user.getName() << " " << user.getState() << std::endl;
        }
    }
    std::cout << "----------------------group list----------------------" << std::endl;
    if (!g_currentUserGroupList.empty())
    {
        for (Group &group: g_currentUserGroupList)
        {
            std::cout << group.getId() << " " << group.getName() << " " << group.getDesc() << std::endl;
            for (GroupUser &user: group.getUser())
            {
                std::cout << user.getId() << " " << user.getName() << " " << user.getState()
                          << " " << user.getRole() << std::endl;
            }
        }
    }
    std::cout << "======================================================" << std::endl;
}


void readTaskHandler(int clientfd)
{
    for (;;)
    {
        char buffer[1024]{0};
        int len = recv(clientfd, buffer, 1024, 0);
        if (-1 == len || 0 == len)
        {
            close(clientfd);
            exit(-1);
        }
        // 接收charserver转发的数据，反序列化输出
        json js = json::parse(buffer);
        if (static_cast<int>(EnMsgType::ONE_CHAT_MSG) == js["msgid"].get<int>())
        {
            std::cout << js["time"].get<std::string>() << "[" << js["id"] << "]" << js["name"].get<std::string>()
                      << " said:" << js["msg"].get<std::string>() << std::endl;
            continue;
        } else if (static_cast<int>(EnMsgType::GROUP_CHAT_MSG) == js["msgid"].get<int>())
        {
            std::cout << "群消息[" << js["groupid"] << "] " << "[" << js["time"].get<std::string>() << "]" << "["
                      << js["id"] << "]"
                      << js["name"].get<std::string>()
                      << " said:" << js["msg"].get<std::string>() << std::endl;
            continue;
        } else if (static_cast<int>(EnMsgType::LOGIN_MSG_ACK) == js["msgid"].get<int>())
        {
            doLoginResponse(js);// 处理登录响应的业务逻辑
            sem_post(&rwsem); // 通知主线程处理完成
            continue;
        } else if (static_cast<int>(EnMsgType::REG_MSG_ACK) == js["msgid"].get<int>())
        {
            doRegResponse(js);// 处理登录响应的业务逻辑
            sem_post(&rwsem); // 通知主线程处理完成
            continue;
        }
    }
}

void doRegResponse(const json &responsejs)
{
    if (responsejs["errno"].get<int>() != 0)
    {
        std::cerr << "name is already exist,register error!" << std::endl;
    } else// 注册成功
    {
        std::cout << "register success,userid is " << responsejs["id"] << " do not forget it"
                  << std::endl;
    }
}

// 处理登陆的响应逻辑
void doLoginResponse(const json &responsejs)
{
    if (responsejs["errno"].get<int>() != 0)//登录失败
    {
        std::cerr << responsejs["errmsg"] << std::endl;
        g_isLoginSuccess = false;
    } else// 登录成功
    {
        // 记录当前用户的id和name
        g_currentUser.setId(responsejs["id"].get<int>());
        g_currentUser.setName(responsejs["name"].get<std::string>());
        // 记录当前用户的好友列表信息
        if (responsejs.contains("friends"))
        {
            std::vector<std::string> vec = responsejs["friends"];
            for (std::string &str: vec)
            {
                json js = json::parse(str);
                User user;
                user.setId(js["id"].get<int>());
                user.setName(js["name"].get<std::string>());
                user.setState(js["state"].get<std::string>());
                g_currentUserFriendList.push_back(user);
            }
        }
        // 记录当前用户群组信息
        if (responsejs.contains("groups"))
        {
            std::vector<std::string> vec1 = responsejs["groups"];
            for (std::string &groupstr: vec1)
            {
                json grpjs = json::parse(groupstr);
                Group group;
                group.setId(grpjs["id"].get<int>());
                group.setName(grpjs["name"].get<std::string>());
                group.setDesc(grpjs["groupdesc"].get<std::string>());
                std::vector<std::string> vec2 = grpjs["users"];
                for (std::string &userstr: vec2)
                {
                    GroupUser user;
                    json js = json::parse(userstr);
                    user.setId(js["id"].get<int>());
                    user.setName(js["name"].get<std::string>());
                    user.setState(js["state"].get<std::string>());
                    user.setRole(js["role"].get<std::string>());
                    group.getUser().push_back(user);
                }
                g_currentUserGroupList.push_back(group);
            }
        }
        // 显示用户基本信息
        showCurrentUserData();
        // 显示当前用户离线消息，个人聊天信息或者群组消息
        if (responsejs.contains("offlinemsg"))
        {
            std::vector<std::string> vec = responsejs["offlinemsg"];
            for (std::string &str: vec)
            {
                json js = json::parse(str);
                if (static_cast<int>(EnMsgType::ONE_CHAT_MSG) == js["msgid"].get<int>())
                {
                    // time +[id] + name +"said:"+xxx
                    std::cout << js["time"] << "[" << js["id"] << "]" << js["name"] << "said"
                              << js["msg"] << std::endl;
                } else
                {
                    std::cout << "群消息[" << js["groupid"] << js["time"].get<std::string>() << "]"
                              << "[" << js["id"] << "]"
                              << js["name"].get<std::string>()
                              << "said:" << js["msg"].get<std::string>() << std::endl;
                }
            }
        }
        g_isLoginSuccess = true;
    }
}

// 获取系统时间(聊天信息需要添加时间信息)
std::string getCurrentTime()
{
    auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    tm *ptm = localtime(&tt);
    char date[60]{0};
    sprintf(date, "%d-%02d-%02d %02d:%02d:%02d", static_cast<int>(ptm->tm_year) + 1900,
            static_cast<int>(ptm->tm_mon) + 1, static_cast<int>(ptm->tm_mday), static_cast<int>(ptm->tm_hour),
            static_cast<int>(ptm->tm_min), static_cast<int>(ptm->tm_sec));
    return static_cast<std::string>(date);
}

void mainMenu(int clientfd)
{
    help();
    char buffer[1024]{0};
    while (isMainMenuRunning)
    {
        std::cin.getline(buffer, 1024);
        std::string commandBuf(buffer);
        std::string command;
        int idx = commandBuf.find(":");
        if (-1 == idx)
        {
            command = commandBuf;
        } else
        {
            command = commandBuf.substr(0, idx);
        }
        auto it = commandHandlerMap.find(command);
        if (it == commandHandlerMap.end())
        {
            std::cerr << "invalid input command!" << std::endl;
            continue;
        }
        it->second(clientfd, commandBuf.substr(idx + 1, commandBuf.size() - idx));
    }

}

void help(int, std::string)
{
    std::cout << "show command list >>>" << std::endl;
    for (auto &[k, v]: commandMap)
    {
        std::cout << k << ":" << v << std::endl;
    }
    std::cout << std::endl;
}

void chat(int clientfd, std::string str)
{
    int idx = str.find(":");
    if (-1 == idx)
    {
        std::cerr << "chat command invalid!" << std::endl;
        return;
    }
    int friendid = atoi(str.substr(0, idx).c_str());
    std::string message = str.substr(idx + 1, str.size() - idx);
    json js;
    js["msgid"] = EnMsgType::ONE_CHAT_MSG;
    js["id"] = g_currentUser.getId();
    js["name"] = g_currentUser.getName();
    js["toid"] = friendid;
    js["msg"] = message;
    js["time"] = getCurrentTime();
    std::string buffer = js.dump();
    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (-1 == len)
    {
        std::cerr << "send char msg error ->" << buffer << std::endl;
    }
}

void addfriend(int clientfd, std::string str)
{
    int friendid = atoi(str.c_str());
    json js;
    js["msgid"] = EnMsgType::ADD_FRIEND_MSG;
    js["id"] = g_currentUser.getId();
    js["friendid"] = friendid;
    std::string buffer = js.dump();
    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (-1 == len)
    {
        std::cerr << "send addfriend msg error ->" << buffer << std::endl;
    }
}

void creategroup(int clientfd, std::string str)
{
    int idx = str.find(":");
    if (-1 == idx)
    {
        std::cerr << "creategroup command invalid!" << std::endl;
    }
    std::string groupname = str.substr(0, idx);
    std::string groupdesc = str.substr(idx + 1, str.size() - idx);
    json js;
    js["msgid"] = EnMsgType::CREATE_GROUP_MSG;
    js["id"] = g_currentUser.getId();
    js["groupname"] = groupname;
    js["groupdesc"] = groupdesc;
    std::string buffer = js.dump();
    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()), 0);
    if (-1 == len)
    {
        std::cerr << "send creategroup msg error->" << buffer << std::endl;
    }

}

void addgroup(int clientfd, std::string str)
{
    int groupid = atoi(str.c_str());
    json js;
    js["msgid"] = EnMsgType::ADD_GROUP_MSG;
    js["id"] = g_currentUser.getId();
    js["groupid"] = groupid;
    std::string buffer = js.dump();
    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()), 0);
    if (-1 == len)
    {
        std::cerr << "send addgroup msg error->" << buffer << std::endl;
    }
}

void groupchat(int clientfd, std::string str)
{
    int idx = str.find(":");
    if (-1 == idx)
    {
        std::cerr << "groupchat command invaild" << std::endl;
    }
    int groupid = atoi(str.substr(0, idx).c_str());
    std::string message = str.substr(idx + 1, str.size() - idx);
    json js;
    js["msgid"] = EnMsgType::GROUP_CHAT_MSG;
    js["id"] = g_currentUser.getId();
    js["name"] = g_currentUser.getName();
    js["groupid"] = groupid;
    js["message"] = message;
    js["time"] = getCurrentTime();
    std::string buffer = js.dump();
    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()), 0);
    if (-1 == len)
    {
        std::cerr << "send gorupchat msg error->" << buffer << std::endl;
    }
}

void loginout(int clientfd, std::string)
{
    json js;
    js["msgid"] = EnMsgType::LOGIN_OUT_MSG;
    js["id"] = g_currentUser.getId();
    std::string buffer = js.dump();
    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()), 0);
    if (-1 == len)
    {
        std::cerr << "send gorupchat msg error->" << buffer << std::endl;
    } else
    {
        g_currentUserGroupList.clear();
        g_currentUserFriendList.clear();
        isMainMenuRunning = false;
    }
}
