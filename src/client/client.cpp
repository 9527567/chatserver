#include "client.hpp"

void showCurrentUserData()
{

}
void readTaskHandler(int clientfd)
{

}
void getCurrentTime()
{

}
void mainMenu()
{

}
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
                js["msg_id"] = EnMsgType::LOGIN_MSG;
                js["id"] = id;
                js["password"] = pwd;
                std::string request = js.dump();
                int len = send(clinetfd, request.c_str(), strlen(request.c_str()) + 1, 0);
                if (len == -1)
                {
                    std::cerr << "send login msg error:" << request << std::endl;
                } else
                {
                    char buffer[1024]{0};
                    len = recv(clinetfd, buffer, 1024, 0);
                    if (len == -1)
                    {
                        std::cerr << "recv login response error" << std::endl;
                    } else
                    {
                        json responsejs = json::parse(buffer);
                        if (responsejs["errno"].get<int>() != 0)//登录失败
                        {
                            std::cerr << responsejs["errmsg"] << std::endl;
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
                                    // time +[id] + name +"said:"+xxx
                                    std::cout << js["time"] << "[" << js["id"] << "]" << js["name"] << "said"
                                              << js["msg"]
                                              << std::endl;
                                }
                            }
                            //登录成功，启动接收线程负责接收数据
                            std::thread readTask(readTaskHandler, clinetfd);
                            readTask.detach();
                            // 进入聊天主页面
                            mainMenu();
                        }
                    }
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
                js["msg_id"] = EnMsgType::REG_MSG;
                js["name"] = name;
                js["password"] = pwd;
                std::string request = js.dump();
                int len = send(clinetfd, request.c_str(), strlen(request.c_str()) + 1, 0);
                if (len == -1)
                {
                    std::cerr << "send reg msg error" << request << std::endl;
                } else
                {
                    char buffer[1024]{0};
                    len = recv(clinetfd, buffer, 1024, 0);
                    if (len == -1)
                    {
                        std::cerr << "recv reg response error" << std::endl;
                    } else
                    {
                        json responsejs = json::parse(buffer);
                        if (responsejs["errno"].get<int>() != 0)
                        {
                            std::cerr << name << "is already exist,register error!" << std::endl;
                        } else// 注册成功
                        {
                            std::cout << "register success,userid is" << responsejs["id"] << "do not forget it"
                                      << std::endl;
                        }
                    }
                }
            }
                break;
            case 3:
            {
                close(clinetfd);
                exit(0);
            }
                break;
            default:
            {
                std::cerr << "Invalid input!" << std::endl;
                break;
            }
        }
    }
}