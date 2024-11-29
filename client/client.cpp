#include "client.hpp"
#include "DoubleLinkList.hpp"

int Client::sock=-1;

extern DoubleLinkList<DataType> list;

extern int c[30];

Client::Client(string name)
{ // 构造函数
    this->name = name;
}

void Client::start_client()
{
    // 创建套接字
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        cout << "无法创建套接字" << endl;
        return;
    }

    // 设置服务器地址
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8888);                       // 服务器端口
    server_address.sin_addr.s_addr = inet_addr("192.168.53.23"); // 服务器IP地址

    // 连接到服务器
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        cout << "无法连接到服务器" << endl;
        close(sock);
        return;
    }

    cout << "已连接到服务器" << endl;

    uint8_t name_type_identifier = 3;                                 // 标识符3，表示用户名
    write(sock, &name_type_identifier, sizeof(name_type_identifier)); // 发送菜名类型标识符
    uint32_t name_size = htonl(name.size());                          // 将长度转换为网络字节序
    // 自定义姓名
    write(sock, &name_size, sizeof(name_size)); // 发送菜名长度
    write(sock, name.c_str(), name.size());

    // 多线程处理
    pthread_t th1;
    pthread_create(&th1, NULL, Client::receivedata, &sock);
    pthread_join(th1, NULL);
    // 关闭套接字
    close(sock);
    return;
}

// 发送数据到服务器
void *Client::senddata(void *arg)
{
    int sock = *(int *)arg;
    char data[200];

    while (1)
    {
        memset(data, 0, 200);
        cout << "请输入要发送的消息：";
        cin >> data;

        int send = write(sock, data, sizeof(data)); // 发送消息
        if (send == -1)
        {
            cout << "发送数据失败" << endl;
        }
    }
}

// 发送链表数据
void Client::send_list()
{
    int i;
    Node<DataType> *p = list.head; // 创建一个指向结点的指针p后面将用这个指针遍历
    for (i = 0; i < list.length; i++)
    {
        cout << "sock数值：" << sock << endl;

        p = p->next;
        if (c[i]>0)
        {
            cout << "sock数值：" << sock << endl;
            // 发送菜名
            uint8_t name_type_identifier = 1;                                 // 标识符，1表示菜名
            write(sock, &name_type_identifier, sizeof(name_type_identifier)); // 发送菜名类型标识符
            uint32_t name_size = htonl(p->data.name.size());                  // 将长度转换为网络字节序
            write(sock, &name_size, sizeof(name_size));               // 发送菜名长度
            write(sock, p->data.name.c_str(), p->data.name.size());   // 发送菜名
            cout<<"要传输的数据来了"<<endl;
            // 发送售出数量
            uint8_t number_type_identifier = 2;                                   // 标识符，4表示售量
            write(sock, &number_type_identifier, sizeof(number_type_identifier)); // 发送售出数量类型标识符
            write(sock, (const char *)&p->data.number, sizeof(p->data.number));   // 发送售出数量
        }
    }

    std::cout << "发送链表数据成功！" << endl;
}

// 接收服务器返回的数据
void *Client::receivedata(void *arg)
{
    int sock = *(int *)arg;
    char buffer[1024];      // 增加缓冲区大小
    char temp_buffer[4096]; // 增加临时缓冲区大小
    int temp_len = 0;
    // 记录链表数据的顺序
    int count = 1;
    int count_number = 0;
    DataType data;

    while (1)
    {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(sock, &read_fds);

        struct timeval timeout;
        timeout.tv_sec = 10; // 设置超时时间为3秒
        timeout.tv_usec = 0;

        int select_result = select(sock + 1, &read_fds, NULL, NULL, &timeout);
        if (select_result == -1)
        {
            perror("select 错误！");
            pthread_exit((void *)-1);
        }
        else if (select_result == 0)
        {
            // 超时，没有数据可读
            count = 1;            // 序号置一
            count_number = 0; // 清零计数值
            std::cout << "计数值已清零" << std::endl;
        }

        memset(buffer, 0, sizeof(buffer));
        int recv_len = read(sock, buffer, sizeof(buffer) - 1); // 接收消息
        if (recv_len == -1)
        {
            cout << "接收服务器消息失败！" << endl;
            break;
        }
        else if (recv_len == 0)
        {
            cout << "服务器掉线了！" << endl;
            break;
        }
        else
        {
            if (count_number == 0 && count == 1)
            {
                list.Clear();
            }
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
                    cout << "收到菜名：" << string(temp_buffer + sizeof(type_identifier) + sizeof(data_size), data_size) << endl;
                    data.name = string(temp_buffer + sizeof(type_identifier) + sizeof(data_size), data_size);
                    // 移动到下一个数据项的起始位置
                    memmove(temp_buffer, temp_buffer + sizeof(type_identifier) + sizeof(data_size) + data_size, temp_len - sizeof(type_identifier) - sizeof(data_size) - data_size);
                    temp_len -= sizeof(type_identifier) + sizeof(data_size) + data_size;
                }
                else if (type_identifier == 2) // 如果标识符为2，表示数据是口味
                {
                    // 读取口味长度
                    uint32_t data_size;
                    memcpy(&data_size, temp_buffer + sizeof(type_identifier), sizeof(data_size));
                    data_size = ntohl(data_size); // 将网络字节序转换为主机字节序

                    // 检查是否有足够的数据来读取完整的口味
                    if (temp_len < sizeof(type_identifier) + sizeof(data_size) + data_size)
                    {
                        break;
                    }
                    count_number++;
                    // 输出完整的口味
                    cout << "收到口味：" << string(temp_buffer + sizeof(type_identifier) + sizeof(data_size), data_size) << endl;
                    data.taste = string(temp_buffer + sizeof(type_identifier) + sizeof(data_size), data_size);
                    // 移动到下一个数据项的起始位置
                    memmove(temp_buffer, temp_buffer + sizeof(type_identifier) + sizeof(data_size) + data_size, temp_len - sizeof(type_identifier) - sizeof(data_size) - data_size);
                    temp_len -= sizeof(type_identifier) + sizeof(data_size) + data_size;
                }
                else if (type_identifier == 3) // 如果标识符为3，表示数据是价格
                {
                    // 检查是否有足够的数据来读取完整的价格
                    if (temp_len < sizeof(type_identifier) + sizeof(int))
                    {
                        break;
                    }
                    count_number++;
                    int received_value;
                    memcpy(&received_value, temp_buffer + sizeof(type_identifier), sizeof(received_value));
                    cout << "收到价格：" << received_value << endl;
                    data.price = received_value;
                    // 移动到下一个数据项的起始位置
                    memmove(temp_buffer, temp_buffer + sizeof(type_identifier) + sizeof(int), temp_len - sizeof(type_identifier) - sizeof(int));
                    temp_len -= sizeof(type_identifier) + sizeof(int);
                }
                else if (type_identifier == 4) // 如果标识符为4，表示数据是售量
                {
                    // 检查是否有足够的数据来读取完整的售出数量
                    if (temp_len < sizeof(type_identifier) + sizeof(int))
                    {
                        break;
                    }
                    count_number++;
                    int received_value;
                    memcpy(&received_value, temp_buffer + sizeof(type_identifier), sizeof(received_value));
                    cout << "收到售量：" << received_value << endl;
                    data.number = received_value;
                    // 移动到下一个数据项的起始位置
                    memmove(temp_buffer, temp_buffer + sizeof(type_identifier) + sizeof(int), temp_len - sizeof(type_identifier) - sizeof(int));
                    temp_len -= sizeof(type_identifier) + sizeof(int);
                }
                else if (type_identifier == 5) // 如果标识符为5，表示数据是售罄
                {
                    // 读取是否售罄长度
                    uint32_t data_size;
                    memcpy(&data_size, temp_buffer + sizeof(type_identifier), sizeof(data_size));
                    data_size = ntohl(data_size); // 将网络字节序转换为主机字节序

                    // 检查是否有足够的数据来读取完整的售罄状态
                    if (temp_len < sizeof(type_identifier) + sizeof(data_size) + data_size)
                    {
                        break;
                    }
                    count_number++;
                    // 输出完整的售罄状态
                    cout << "收到售罄状态：" << string(temp_buffer + sizeof(type_identifier) + sizeof(data_size), data_size) << endl;
                    if (strcmp("已售罄", string(temp_buffer + sizeof(type_identifier) + sizeof(data_size), data_size).c_str()) == 0)
                    {
                        data.empty = 1;
                    }
                    else
                    {
                        data.empty = 0;
                    }
                    // 移动到下一个数据项的起始位置
                    memmove(temp_buffer, temp_buffer + sizeof(type_identifier) + sizeof(data_size) + data_size, temp_len - sizeof(type_identifier) - sizeof(data_size) - data_size);
                    temp_len -= sizeof(type_identifier) + sizeof(data_size) + data_size;
                }
                else if (type_identifier == 6) // 如果标识符为6，表示数据是图片
                {
                    // 读取图片路径长度
                    uint32_t data_size;
                    memcpy(&data_size, temp_buffer + sizeof(type_identifier), sizeof(data_size));
                    data_size = ntohl(data_size); // 将网络字节序转换为主机字节序

                    // 检查是否有足够的数据来读取完整的图片路径
                    if (temp_len < sizeof(type_identifier) + sizeof(data_size) + data_size)
                    {
                        break;
                    }
                    count_number++;
                    // 输出完整的图片路径
                    cout << "收到图片路径：" << string(temp_buffer + sizeof(type_identifier) + sizeof(data_size), data_size) << endl;
                    data.picture = string(temp_buffer + sizeof(type_identifier) + sizeof(data_size), data_size);
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

                if (count_number == 6)
                {
                    // 修改链表数据
                    list.Insert(data, count);
                    count++;
                    count_number = 0;
                    list.PrintAll();
                    cout << data.name << endl;
                }
            }
        }
    }
    return 0;
}

void *client_start(void *arg)
{
    Client client("纳兰");
    client.start_client();
}