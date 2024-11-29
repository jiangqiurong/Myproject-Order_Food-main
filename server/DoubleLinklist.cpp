#include <iostream>
#include "DoubleLinkList.hpp"
#include "server.hpp"
// 初始化————初始化一个空表（只有头节点）
template <typename DataType>
DoubleLinkList<DataType>::DoubleLinkList()
{
    head = new Node<DataType>; // 动态分配一个空结点，即头节点
    assert(head);
    head->prior = head; // 头节点的首指针指向其本身
    head->next = head;  // 头节点的尾指针指向其本身
    length = 0;
}

// 初始化————初始化一个含有数据的表
// 实参用数组传入要写入的数据
// 由于C++中没有自带直接求数组长度的函数
// 所以需要手动把数组的长度n写进去
template <typename DataType>
DoubleLinkList<DataType>::DoubleLinkList(const DataType data_[], int n)
{
    head = new Node<DataType>; // 动态分配一个空结点，即头节点
    Node<DataType> *p = head;  // 创建一个指向结点的指针p后面将用这个指针遍历，实现不断的加数据结点
    for (int i = 0; i < n; i++)
    {
        // 这两行代码其实包含了四个操作
        // step1 :new运算符建立下一个结点 且通过构造函数将数据域赋值为data_[i]
        // step2:通过构造函数将该结点的首指针指向p（即上一个节点） 尾指针指向NULL
        // step3:通过赋值运算符将上一个结点的尾指针指向下一个结点
        // step4 将指针p移动至下一个结点 以便下一步的迭代
        p->next = new Node<DataType>(data_[i], p, NULL);
        p = p->next;
    }
    length = n;
    p->next = head;
    head->prior = p;
}

std::ostream &operator<<(std::ostream &os, const DataType &dt)
{

    // 计算并添加空格

    std::string namePadding(15 - dt.name.size() / 3 * 2, ' ');
    std::string tastePadding(10 - dt.taste.size() / 3 * 2, ' ');
    std::string pricePadding(4 - to_string(dt.price).size(), ' ');
    std::string numberPadding(6 - to_string(dt.number).size(), ' ');
    std::string emptyPadding(5, ' ');

    // 右对齐
    os << std::left;
    os << " 菜名:" << dt.name << namePadding << " 口味:" << dt.taste << tastePadding
       << " 价格:" << dt.price << pricePadding << "元 售出份数:" << dt.number << numberPadding
       << " 是否售罄:" << (dt.empty ? "已售罄" : "未售罄") << emptyPadding << " 图片路径:" << dt.picture;
    return os;
}

bool operator!=(const DataType &dt, const DataType &other)
{
    return dt.name != other.name || dt.taste != other.taste || dt.price != other.price || dt.number != other.number || dt.empty != other.empty ||
           dt.picture != other.picture;
}

// 增————向某个位置pos插入一个数据data_
template <typename DataType>
void DoubleLinkList<DataType>::Insert(const DataType data_, int pos)
{
    // step0:准备工作 建立要遍历的指针p和新建要插入的数据结点add
    Node<DataType> *p = head;                                    // 创建一个指向结点的指针p后面将用这个指针遍历
    Node<DataType> *add = new Node<DataType>(data_, NULL, NULL); // 建立一个新结点，包含要插入的数据data_
    // step1:判断插入位置是否正确
    if (pos < 1 || pos > length + 1)
    {
        std::cout << "插入数据失败" << endl;
    }

    else
    {
        // step2,用指针p进行遍历，直到达到指定位置
        int i;
        for (i = 0; i < pos; i++)
        {
            p = p->next;
        }
        // step3 ,开始插入
        add->prior = p->prior;
        p->prior->next = add;
        p->prior = add;
        add->next = p;
        length += 1;
        std::cout << "插入数据成功" << endl;
    }
}

// 删————删除某个位置pos的结点
template <typename DataType>
void DoubleLinkList<DataType>::Delete(int pos)
{
    // step0:准备工作 建立要遍历的指针p和新建要插入的数据结点add
    Node<DataType> *p = head; // 创建一个指向结点的指针p后面将用这个指针遍历
    // step1:判断删除位置是否正确
    if (pos < 1 || pos > length)
    {
        std::cout << "删除数据失败" << endl;
    }
    else
    {
        int i;
        for (i = 0; i < pos; i++)
        {
            p = p->next;
        }
        // step2:开始删除
        p->prior->next = p->next;
        p->next->prior = p->prior;
        length = length - 1;
        std::cout << "删除数据成功" << endl;
        delete p; // 千万注意此处要delete指针p!
    }
}

// 改————改动某个位置pos处的结点
template <typename DataType>
void DoubleLinkList<DataType>::Change(const DataType data_, int pos)
{
    // step0:准备工作 建立要遍历的指针p和新建要改动的数据结点add
    Node<DataType> *p = head; // 创建一个指向结点的指针p后面将用这个指针遍历

    // step1:判断插入位置是否正确
    if (pos < 1 || pos > length + 1)
    {
        std::cout << "改动数据失败" << endl;
    }

    else
    {
        int i;
        // step2,用指针p进行遍历，直到达到指定位置
        for (i = 0; i < pos; i++)
        {
            p = p->next;
        }
        // step3 ,开始改动
        p->data = data_;
        std::cout << "改动数据成功" << endl;
    }
}

// 查————根据下标查数据
template <typename DataType>
DataType DoubleLinkList<DataType>::Search1(int pos)
{
    // step0:准备工作 建立要遍历的指针p
    Node<DataType> *p = head; // 创建一个指向结点的指针p后面将用这个指针遍历
    // step1:判断查找位置是否正确
    if (pos < 1 || pos > length)
    {
        std::cout << "查找数据失败" << endl;
        return DataType();
    }
    else
    {
        int i;
        for (i = 0; i < pos; i++)
        {
            p = p->next;
        }
        // step2:返回查找数据
        std::cout << "查找数据成功" << endl;
        return p->data;
    }
}

// 查————根据数据查下标
template <typename DataType>
int DoubleLinkList<DataType>::Search2(const DataType &e)
{
    // step0:准备工作 建立要遍历的指针p
    Node<DataType> *p = head; // 创建一个指向结点的指针p后面将用这个指针遍历
    // step1:开始查找
    int i = 0;
    while ((i <= length) && (p->data.name != e.name && p->data.taste != e.taste))
    {
        i++;
        p = p->next;
    }
    if (p == head)
    {
        std::cout << "找不到该数据" << endl;
        return 0;
    }
    else
    {
        std::cout << "成功找到该数据" << endl;
        cout << "编号：" << i << " 菜名：" << p->data.name << " 口味：" << p->data.taste << " 价格：" << p->data.price << "元 售出份数：" << p->data.number << " 是否售罄：" << (p->data.empty ? "已售罄" : "未售罄") << " 图片路径：" << p->data.picture << endl;
        return i;
    }
}

// 清空
template <typename DataType>
void DoubleLinkList<DataType>::Clear()
{
    Node<DataType> *p = head->next;
    while (p != head)
    {
        Node<DataType> *temp = p;
        p = p->next;
        delete temp;
    }
    head->next = head;
    head->prior = head;
    length = 0;
}

// 析构
template <typename DataType>
DoubleLinkList<DataType>::~DoubleLinkList()
{
    Clear();
    delete head;
    cout << "析构函数已执行" << endl;
}

// 求线性表长度
template <typename DataType>
int DoubleLinkList<DataType>::GetLength()
{
    return length;
}

// 遍历输出
template <typename DataType>
void DoubleLinkList<DataType>::PrintAll()
{
    int i;
    Node<DataType> *p = head; // 创建一个指向结点的指针p后面将用这个指针遍历
    for (i = 0; i < length; i++)
    {
        p = p->next;
        cout << "编号:" << (i + 1) << p->data << " ";
        cout << endl;
    }

    cout << endl;
}

// 出售记录
template <typename DataType>
void DoubleLinkList<DataType>::SaleRecords()
{
    int i;
    int countnumber = 0;
    int countprice = 0;
    Node<DataType> *p = head; // 创建一个指向结点的指针p后面将用这个指针遍历
    cout << "销售记录如下" << endl;
    for (i = 0; i < length; i++)
    {
        p = p->next;
        std::string namePadding(15 - p->data.name.size() / 3 * 2, ' ');
        std::string pricePadding(4 - to_string(p->data.price).size(), ' ');
        cout << "编号:" << (i + 1) << p->data.name << namePadding << " 价格:" << p->data.price << pricePadding << "元 售出份数:" << p->data.number << "份" << endl;
        countnumber += p->data.number;
        countprice += p->data.number * p->data.price;
    }
    cout << "总计销售:" << countnumber << "份 总计销售金额:" << countprice << "元" << endl;
}

//-----------------------------------------------------------------------以上为模板区域
// 清屏
void clearorders()
{
    int c = 0;
    cout << "按任意键返回......" << endl;
    while ((c = getchar()) != '\n')
        ;
    {
        getchar();
    }
    return;
}

// 创建主界面
void Menu()
{
    std::cout << "菜品系统，启动！" << std::endl;
    std::cout << "=== 菜品管理系统 ==" << std::endl;
    std::cout << "=1. 添加菜品信息  =" << std::endl;
    std::cout << "=2. 查询菜品信息  =" << std::endl;
    std::cout << "=3. 修改菜品信息  =" << std::endl;
    std::cout << "=4. 删除菜品信息  =" << std::endl;
    std::cout << "=5. 修改菜品售罄  =" << std::endl;
    std::cout << "=6. 恢复菜品出售  =" << std::endl;
    std::cout << "=7. 统计出售情况  =" << std::endl;
    std::cout << "=8. 发送最新列表  =" << std::endl;
    std::cout << "=9. 刷新列表      =" << std::endl;
    std::cout << "=0. 退出          =" << std::endl;
    std::cout << "===================" << std::endl;
    return;
}

DoubleLinkList<DataType> list;

    // 功能选择
void choose()
{
    // 初始化6种菜品
    DataType food1 = {"回锅肉", "咸鲜", 18, 0, 0, "./foodpicture/food1.bmp"};
    DataType food2 = {"狮子头", "鲜香", 20, 0, 0, "./foodpicture/food2.bmp"};
    DataType food3 = {"蒸水蛋", "爽滑", 12, 0, 0, "./foodpicture/food3.bmp"};
    DataType food4 = {"麻婆豆腐", "麻辣", 15, 0, 1, "./foodpicture/food4.bmp"};
    DataType food5 = {"番茄蛋汤", "浓香", 10, 0, 0, "./foodpicture/food5.bmp"};
    DataType food6 = {"酱汁秋葵", "爽脆", 17, 0, 0, "./foodpicture/food6.bmp"};

    list.Insert(food1, 1);
    list.Insert(food2, 2);
    list.Insert(food3, 3);
    list.Insert(food4, 4);
    list.Insert(food5, 5);
    list.Insert(food6, 6);

    int choice = 0;
    int number = 0;
    do
    {
        system("clear");
        Menu();
        cout << "菜品列表如下:" << endl;
        list.PrintAll();
        cout << "请输入选项(0~9):" << endl;
        cin >> choice;
        switch (choice)
        {
        // 1.添加菜品信息
        case 1:
        {
            DataType food;
            // 检查是否有编号重复
            cout << "请输入菜品要插入的编号:" << endl;
            cin >> food.number;
            cout << "请输入菜品名称:" << endl;
            cin >> food.name;
            cout << "请输入口味:" << endl;
            cin >> food.taste;
            cout << "请输入价格:" << endl;
            cin >> food.price;
            cout << "请输入是否售罄(1表示已售罄,0表示未售罄):" << endl;
            cin >> food.empty;
            cout << "请输入菜品图片路径:" << endl;
            cin >> food.picture;
            list.Insert(food, food.number);
            cout << "列表更新如下：" << endl;
            list.PrintAll();
            // 清屏
            clearorders();
        };
        break;
        // 2. 查询菜品信息
        case 2:
        {
            int choice2 = 0;
            DataType data;
        c2:
            if (cin.fail())
            {
                cin.clear();                                         // 清除错误状态
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略错误输入之后的所有字符
            }
            cout << "请选择查找方式:1.编号 2.名称或口味" << endl;
            cin >> choice2;
            if (choice2 == 1)
            {

                cout << "请输入要查找的编号:" << endl;
                cin >> number;
                data = list.Search1(number);
                cout << "编号：" << number << " 菜名：" << data.name << " 口味：" << data.taste << " 价格：" << data.price << " 售出份数：" << data.number << " 是否售罄：" << (data.empty ? "已售罄" : "未售罄") << " 图片路径：" << data.picture << endl;
            }
            else if (choice2 == 2)
            {

                cout << "请输入要查找的名称或口味:" << endl;
                cin >> data.name;
                data.taste = data.name;
                list.Search2(data);
            }
            else
            {
                cout << "输入错误，请重新输入！" << endl;
                goto c2;
            }
            clearorders();
        };
        break;
        // 3. 修改菜品信息
        case 3:
        {
            cout << "请输入要修改的菜品编号:" << endl;
            cin >> number;
            DataType data = list.Search1(number);

            if (data.name != "")
            {
                cout << "请输入新的菜品名称:" << endl;
                cin >> data.name;
                cout << "请输入新的口味:" << endl;
                cin >> data.taste;
                cout << "请输入新的价格:" << endl;
                cin >> data.price;
                cout << "请输入新的是否售罄(1表示已售罄,0表示未售罄):" << endl;
                cin >> data.empty;
                cout << "请输入新的菜品图片路径:" << endl;
                cin >> data.picture;
                list.Change(data, number);
                cout << "列表更新如下：" << endl;
                list.PrintAll();
            }
            clearorders();
        };
        break;
        // 4. 删除菜品信息
        case 4:
        {
            cout << "请输入要删除的菜品编号:" << endl;
            cin >> number;
            list.Delete(number);
            cout << "列表更新如下：" << endl;
            list.PrintAll();
            clearorders();
        };
        break;
        // 5. 修改菜品售罄
        case 5:
        {
            cout << "请输入要修改的菜品编号:" << endl;
            cin >> number;
            DataType data = list.Search1(number);
            if (data.empty == 0)
            {
                data.empty = 1;
                list.Change(data, number);
                cout << "列表更新如下：" << endl;
                list.PrintAll();
            }
            else
            {
                cout << "该菜品已售罄！" << endl;
            }
            clearorders();
        };
        break;
        // 6. 恢复菜品出售
        case 6:
        {
            cout << "请输入要恢复的菜品编号:" << endl;
            cin >> number;
            DataType data = list.Search1(number);
            if (data.empty == 1)
            {
                data.empty = 0;
                list.Change(data, number);
                cout << "列表更新如下：" << endl;
                list.PrintAll();
            }
            else
            {
                cout << "该菜品未售罄！" << endl;
            }
            clearorders();
        };
        break;
        // 7. 统计出售情况
        case 7:
        {
            list.SaleRecords();
            clearorders();
        };
        break;
        // 8. 发送最新列表
        case 8:
        {
            Server::send_list(); // 发送链表数据
            clearorders();
        };
        break;
        // 9. 刷新列表
        case 9:
        {
            clearorders();
        };
        break;
        // 退出
        case 0:
            printf("菜品管理系统已关闭！\n");
            break;
        }
    } while (choice != 0);

    return;
}

void *list_start(void *arg)
{
    //DoubleLinkList<DataType> &list = *static_cast<DoubleLinkList<DataType> *>(arg);
    // 功能选择
    choose();

    return 0;
}
