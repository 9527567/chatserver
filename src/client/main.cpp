#include "nlohmann/json.hpp"
#include <iostream>
int main(int argc,char** argv)
{
    if (argc < 3)
    {
        std::cerr << "command invalid! example ./chatClient 127.0.0.1 6000" << std::endl;
    }
}