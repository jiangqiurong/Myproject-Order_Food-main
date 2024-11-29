#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <iostream>
#include <sys/select.h>

using namespace std;

// TCP服务器类
class Server
{
protected:
    static int sockfd;       // 服务器套接字
    int port;                // 服务器端口号
    string ip;               // IP地址
    static int clientnum;    // 客户端数量
    static int clientfd[10]; // 客户端套接字

public:
    Server(int port, string ip = "");   // 构造函数
    ~Server();                          // 析构函数
    void start();                       // 初始化服务器
    static void run(int clientfd); // 运行服务器
    void send_msg(string msg);          // 发送消息给客户端
    static void recv_msg(int clientfd); // 接收客户端消息
    // 接收服务器返回的数据
    static void receivedata(int client_fd);
    static void send_list(); // 发送链表
};

void *server(void *arg);

#endif
