#include "server.hpp"
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread> // 添加多线程的头文件
#include "DoubleLinkList.hpp"

int Server::sockfd = -1;
int Server::clientfd[10] = {0};
int Server::clientnum = 0;

extern DoubleLinkList<DataType> list;

// 构造函数
Server::Server(int Port, string Ip)
    : port(Port), ip(Ip)
{
    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cout << "创建套接字失败！" << endl;
        return;
    }

    // 服务器绑定套接字： 把服务器的IP地址 和端口号 + 家族协议 绑定到外网把你的内网IP转成内存IP
    //  IP地址分为： 公网IP + 内网IP，
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(Port); // host主机字节序  network网络字节序

    if (ip == "")
    {
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 绑定所有地址,注意不能只绑定一个IP地址，我们电脑是多网卡的，有多个IP
    }
    else
    {
        if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0)
        {
            std::cout << "无效的IP地址！" << endl;
            close(sockfd);
            return;
        }
    }

    // 绑定套接字
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cout << "服务器绑定套接字失败！" << endl;
        close(sockfd);
        return;
    }
    else
    {
        // cout << "服务器绑定套接字成功！" << endl;
    }

    // 监听套接字
    if (listen(sockfd, 10) == -1)
    {
        std::cout << "服务器监听套接字失败！" << endl;
        close(sockfd);
        return;
    }
    else
    {
        // cout << "服务器监听套接字成功！" << endl;
    }
    std::cout << "服务器初始化成功！" << endl;
}

Server::~Server()
{
    // 关闭套接字
    close(sockfd);
    for (int i = 0; i < clientnum; i++)
    {
        close(clientfd[i]);
    }
    std::cout << "服务器关闭成功！" << endl;
}

void Server::start()
{
    // 获得客户端套接字
    thread t1(&Server::run, 0);
    t1.detach(); // 线程分离，不用等待线程结束才退出程序
}

void Server::run(int client_fd)
{
    // 服务器要调用一个函数去等待客户端来连接他
    struct sockaddr_in client_addr;
    socklen_t size = sizeof(client_addr);

    while (true)
    {
        memset(&client_addr, 0, size);
        int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &size); // 第二个和第三个参数表示获取连接的客户端的网络信息
        if (client_fd == -1)
        {
            std::cout << "服务器接受客户端连接失败！" << endl;
            continue;
        }
        else
        {
            if (clientnum >= 10)
            {
                std::cout << "客户端数量已达上限，无法接受新连接！" << endl;
                close(client_fd);
                continue;
            }
            clientfd[clientnum++] = client_fd; // 客户端套接字
            // cout << "有人找你!他的IP是:" << inet_ntoa(client_addr.sin_addr) << endl;
            // Server::send_list(client_fd); // 发送链表数据
            thread t1(&Server::receivedata, client_fd);
            // t1.detach(); // 线程分离，不用等待线程结束才退出程序
            t1.join(); // 等待线程结束才退出程序
        }
    }
}

// 发送消息
void Server::send_msg(string msg)
{
    for (int i = 0; i < clientnum; i++)
    {
        int send = write(clientfd[i], msg.c_str(), msg.size()); // 发送消息
        std::cout << "发送消息" << msg << "成功！" << endl;
    }
}

// 接收消息
void Server::recv_msg(int client_fd)
{
    char recv_buf[200];
    while (true)
    {
        memset(recv_buf, 0, sizeof(recv_buf));
        int recv_len = read(client_fd, recv_buf, sizeof(recv_buf)); // 接收消息
        if (recv_len == -1)
        {
            std::cout << "服务器接收客户端消息失败！" << endl;
            break;
        }
        else if (recv_len == 0)
        {
            std::cout << "客户端主动断开连接！" << endl;
            break;
        }
        else
        {
            std::cout << "收到信息：" << recv_buf << endl;
        }
    }
    close(client_fd);
}

// 接收客户端返回的数据
void Server::receivedata(int client_fd)
{
    char buffer[1024];      // 增加缓冲区大小
    char temp_buffer[4096]; // 增加临时缓冲区大小
    int temp_len = 0;
    // 记录链表数据的积累
    int count_number = 0;
    DataType data;

    while (1)
    {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(client_fd, &read_fds);

        struct timeval timeout;
        timeout.tv_sec = 10; // 设置超时时间为3秒
        timeout.tv_usec = 0;

        int select_result = select(client_fd + 1, &read_fds, NULL, NULL, &timeout);
        if (select_result == -1)
        {
            perror("select 错误！");
            pthread_exit((void *)-1);
        }
        else if (select_result == 0)
        {
            count_number = 0; // 清零计数值
            std::cout << "计数值已清零" << std::endl;
        }

        memset(buffer, 0, sizeof(buffer));
        int recv_len = read(client_fd, buffer, sizeof(buffer) - 1); // 接收消息
        if (recv_len == -1)
        {
            cout << "接收客户端消息失败！" << endl;
            break;
        }
        else if (recv_len == 0)
        {
            cout << "客户端主动断开连接！" << endl;
            break;
        }
        else
        {
            // 将接收到的数据追加到临时缓冲区
            memcpy(temp_buffer + temp_len, buffer, recv_len);
            temp_len += recv_len;

            // 处理临时缓冲区中的数据
            while (temp_len > 0)
            {
                // 检查是否有足够的数据来读取类型标识符
                if (temp_len < sizeof(uint8_t))
                {
                    break;
                }

                uint8_t type_identifier;
                memcpy(&type_identifier, temp_buffer, sizeof(type_identifier));

                if (type_identifier == 1) // 如果标识符为1，表示数据是菜名
                {

                    // 读取菜名长度
                    uint32_t data_size;
                    memcpy(&data_size, temp_buffer + sizeof(type_identifier), sizeof(data_size));
                    data_size = ntohl(data_size); // 将网络字节序转换为主机字节序
                    // 检查是否有足够的数据来读取完整的菜名
                    if (temp_len < sizeof(type_identifier) + sizeof(data_size) + data_size)
                    {
                        break;
                    }
                    count_number++;
                    // 输出完整的菜名
                    // cout << "收到菜名：" << string(temp_buffer + sizeof(type_identifier) + sizeof(data_size), data_size) << endl;
                    data.name = string(temp_buffer + sizeof(type_identifier) + sizeof(data_size), data_size);
                    // 移动到下一个数据项的起始位置
                    memmove(temp_buffer, temp_buffer + sizeof(type_identifier) + sizeof(data_size) + data_size, temp_len - sizeof(type_identifier) - sizeof(data_size) - data_size);
                    temp_len -= sizeof(type_identifier) + sizeof(data_size) + data_size;
                }
                else if (type_identifier == 2) // 如果标识符为2，表示数据是售量
                {

                    // 检查是否有足够的数据来读取完整的售出数量
                    if (temp_len < sizeof(type_identifier) + sizeof(int))
                    {
                        break;
                    }
                    count_number++;
                    int received_value;
                    memcpy(&received_value, temp_buffer + sizeof(type_identifier), sizeof(received_value));
                    // cout << "收到售量：" << received_value << endl;
                    data.number = received_value;
                    // 移动到下一个数据项的起始位置
                    memmove(temp_buffer, temp_buffer + sizeof(type_identifier) + sizeof(int), temp_len - sizeof(type_identifier) - sizeof(int));
                    temp_len -= sizeof(type_identifier) + sizeof(int);
                }
                else if (type_identifier == 3)
                {

                    // 读取用户名长度
                    uint32_t data_size;
                    memcpy(&data_size, temp_buffer + sizeof(type_identifier), sizeof(data_size));
                    data_size = ntohl(data_size); // 将网络字节序转换为主机字节序

                    // 检查是否有足够的数据来读取完整的菜名
                    if (temp_len < sizeof(type_identifier) + sizeof(data_size) + data_size)
                    {
                        break;
                    }
                    count_number++;

                    // 输出完整的用户名
                    cout << "👤客户：" << string(temp_buffer + sizeof(type_identifier) + sizeof(data_size), data_size) << " 已上线" << endl;
                    // 移动到下一个数据项的起始位置
                    memmove(temp_buffer, temp_buffer + sizeof(type_identifier) + sizeof(data_size) + data_size, temp_len - sizeof(type_identifier) - sizeof(data_size) - data_size);
                    temp_len -= sizeof(type_identifier) + sizeof(data_size) + data_size;
                }
                else
                {
                    cout << "未知的数据类型标识符" << endl;
                    // 移动到下一个数据项的起始位置
                    memmove(temp_buffer, temp_buffer + sizeof(type_identifier), temp_len - sizeof(type_identifier));
                    temp_len -= sizeof(type_identifier);
                }

                if (count_number == 2)
                {
                    count_number = 0;
                    Node<DataType> *p = list.head; // 创建一个指向结点的指针p后面将用这个指针遍历
                    int i = 0;
                    while ((i <= list.length) && (p->data.name != data.name))
                    {
                        i++;
                        p = p->next;
                    }
                    if (p == list.head)
                    {
                        std::cout << "有客户打印订单错误" << endl;
                        return;
                    }
                    else
                    {
                        p->data.number += data.number;
                    }
                }
            }
        }
    }
    return;
}

void *server(void *arg)
{
    Server server(8888); // 服务器初始化
    server.start();      // 服务器开始运行

    while (1)
        ;
}

// 发送链表数据
void Server::send_list()
{
    int i;
    int j;
    Node<DataType> *p = list.head; // 创建一个指向结点的指针p后面将用这个指针遍历
    for (j = 0; j < clientnum; j++)
    {
        for (i = 0; i < list.length; i++)
        {
            p = p->next;

            // 发送菜名
            uint8_t name_type_identifier = 1;                                        // 标识符，1表示菜名
            write(clientfd[j], &name_type_identifier, sizeof(name_type_identifier)); // 发送菜名类型标识符
            uint32_t name_size = htonl(p->data.name.size());                         // 将长度转换为网络字节序
            write(clientfd[j], &name_size, sizeof(name_size));                       // 发送菜名长度
            write(clientfd[j], p->data.name.c_str(), p->data.name.size());           // 发送菜名

            // 发送口味
            uint8_t taste_type_identifier = 2;                                         // 标识符，2表示口味
            write(clientfd[j], &taste_type_identifier, sizeof(taste_type_identifier)); // 发送口味类型标识符
            uint32_t taste_size = htonl(p->data.taste.size());                         // 将长度转换为网络字节序
            write(clientfd[j], &taste_size, sizeof(taste_size));                       // 发送口味长度
            write(clientfd[j], p->data.taste.c_str(), p->data.taste.size());           // 发送口味

            // 发送价格
            uint8_t price_type_identifier = 3;                                         // 标识符，3表示价格
            write(clientfd[j], &price_type_identifier, sizeof(price_type_identifier)); // 发送价格类型标识符
            write(clientfd[j], (const char *)&p->data.price, sizeof(p->data.price));   // 发送价格

            // 发送售出数量
            uint8_t number_type_identifier = 4;                                          // 标识符，4表示售量
            write(clientfd[j], &number_type_identifier, sizeof(number_type_identifier)); // 发送售出数量类型标识符
            write(clientfd[j], (const char *)&p->data.number, sizeof(p->data.number));   // 发送售出数量

            // 发送是否售罄
            uint8_t sold_out_type_identifier = 5;                                            // 标识符，5表示售罄
            write(clientfd[j], &sold_out_type_identifier, sizeof(sold_out_type_identifier)); // 发送是否售罄类型标识符
            const char *sold_out_status = p->data.empty ? "已售罄" : "未售罄";
            uint32_t sold_out_size = htonl(strlen(sold_out_status));      // 将长度转换为网络字节序
            write(clientfd[j], &sold_out_size, sizeof(sold_out_size));    // 发送是否售罄长度
            write(clientfd[j], sold_out_status, strlen(sold_out_status)); // 发送是否售罄

            // 发送图片路径
            uint8_t picture_type_identifier = 6;                                           // 标识符，6表示图片
            write(clientfd[j], &picture_type_identifier, sizeof(picture_type_identifier)); // 发送图片路径类型标识符
            uint32_t picture_size = htonl(p->data.picture.size());                         // 将长度转换为网络字节序
            write(clientfd[j], &picture_size, sizeof(picture_size));                       // 发送图片路径长度
            write(clientfd[j], p->data.picture.c_str(), p->data.picture.size());           // 发送图片路径
        }
    }
    std::cout << "发送链表数据成功！" << endl;
}
