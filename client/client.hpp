#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

using namespace std;


class Client 
{
public:
    static int sock;
    string name;

    Client(string name="");

    // 启动客户端
    void start_client();
    // 发送数据
    static void* senddata(void* arg);
    // 发送链表数据
    static void send_list();
    // 接收数据
    static void *receivedata(void *arg);
};

void *client_start(void *arg);

#endif