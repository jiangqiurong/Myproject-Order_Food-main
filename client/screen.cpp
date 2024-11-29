#include "screen.hpp"
#include <pthread.h>
#include "DoubleLinkList.hpp"
#include "client.hpp"

extern DoubleLinkList<DataType> list;

// 全局显示文字类对象
MyFont text(800, 480);

using namespace std;

// 广告标志位
volatile bool adThreadActive = true;

// 购买标志位
volatile bool buy = false;

// 页数
int page = 1;

// 按钮数组
Button buttons[4] = {};
Button buttons0[10] = {};
Button buttons1[2] = {};

// 定义全局购买计数数组(最多30道菜)
int c[30] = {0};

// 按键屏幕混合类
class Mix
{
public:
    Button regist;
    static LcdDevice lcd;
    Mix(Button regist, LcdDevice lcd) : regist(regist)
    {
        this->lcd = lcd;
    }

    // 按键按下反馈相关事件
    static void *aj(void *arg)
    {
        // 获取触摸屏数据
        struct TsDevice *ts = createTs();
        while (1)
        {

            struct Point *p = ts->getXY();
            // 判断是否点击了按钮

            for (int i = 0; i < 4; i++)
            {
                if (pointOnButton(p, buttons[i])) // 判断p是否点击在按钮上
                {
                    // 按钮被点击，停止广告线程
                    adThreadActive = false;
                    // 按钮被点击后，设置标志位为真，使所有按钮失效
                    buttons[i].event(lcd); // 执行按钮点击的事件
                }
            }
        }
    }
};

LcdDevice Mix::lcd;

// 判断按钮是否被按下
bool pointOnButton(struct Point *p, Button &button)
{
    // cout << button.x << " " << button.y << " " << button.w << " " << button.h << endl;
    if (p->x > button.x && p->y > button.y)
    {

        if (p->x < (button.x + button.w) && p->y < (button.y + button.h))
        {

            return true;
        }
        else
            return false;
    }
    else
        return false;
}

// 广告
void *ad(void *args)
{
    // LcdDevice lcd = (LcdDevice &)args;
    LcdDevice *lcd = reinterpret_cast<LcdDevice *>(args);
    // font *f = fontLoad("./simfang.ttf"); // 导入字库
    font *f = fontLoad(const_cast<char *>("./simfang.ttf"));
    if (f == NULL)
        perror("fontLoad");

    // 设置字体大小
    fontSetSize(f, 50); // 一个汉字占用32行32列

    // 创建一个区域存储汉字点阵
    bitmap *bitm = createBitmap(800, 480, 4);

    // 从屏幕提取背景填充函数点阵区域
    unsigned char buffer[800 * 480 * 4] = {}; // 提取屏幕的背景暂存

    unsigned int *p = lcd->mptr;

    unsigned char *q = buffer;
    // unsigned char *q = bitm->map;
    for (int i = 0; i < 480; i++)
    {
        memcpy(q, p, 800 * 4);
        q += 800 * 4;
        p += lcd->width;
    }
    int x = 780;

    while (1)
    {

        if (!adThreadActive)
        {
            // 广告线程失效，退出循环
            break;
        }
        memcpy(bitm->map, buffer, 800 * 480 * 4);
        // 把汉字输出到点阵
        fontPrint(f, bitm, x--, 2, const_cast<char *>("中华小食堂成立50周年大酬宾，现在办理VIP可领取专属福利！"), 0xFF000000, 0); // #RGBA
        // 把汉字点阵区域绘制在lcd上
        Bmp bm;
        bm.width = bitm->width;
        bm.height = bitm->height;
        bm.pixel = bitm->byteperpixel;
        bm.data = bitm->map;
        lcd->draw_rgb(0, 10, bm);
        if (x < -960)
            x = 780;
        usleep(200);
        // cout<<"广告滚屏中..."<<endl;
    }

    // 释放点阵空间
    destroyBitmap(bitm);
    // 释放字库
    fontUnload(f);
}

// 背景
void picture(const char *bmp, LcdDevice &lcd, int x, int y, int w, int h)
{
    // 读取BMP
    Bmp bm(bmp);
    bm.zoomBmp(w, h);
    lcd.draw_rgb(x, y, bm);
    bm.destroyBmp();
}

// 关闭主界面对应按钮
void close_menu()
{
    buttons[0].destroyButton();
    buttons[1].destroyButton();
    buttons[2].destroyButton();
    buttons[3].destroyButton();
}

// 按键事件0(开始点菜)
void function0(LcdDevice &lcd)
{
    buy = false;
    //  背景1
    picture("./foodpicture/b1.bmp", lcd, 0, 0, 800, 480);
    // 后续接上链表操作Food *pos = NULL;
    int x = 50, y = 10;

    int n = 0;
    int i = 0;
    // MyFont text(800, 480);
    Node<DataType> *p = list.head; // 创建一个指向结点的指针p后面将用这个指针遍历
    for (n = (page - 1) * 3; ((n < list.length) && n < ((page - 1) * 3 + 3)); n++)
    {
        p = p->next;
        for (i; i < list.length; i++)
        {
            if (i == ((page - 1) * 3))
            {
                i = list.length;
                break;
            }
            p = p->next;
        }
        char buffer[20];
        char buffer1[20];
        snprintf(buffer, sizeof(buffer), "%d", p->data.price);
        snprintf(buffer1, sizeof(buffer1), "%d", c[n]);
        picture(p->data.picture.c_str(), lcd, x, y, 200, 150);
        // 显示
        text.size(lcd, p->data.name, x + 30, y + 160, 40, 0xf2652200);
        text.size(lcd, p->data.taste, x + 30, y + 200, 40, 0xf2652200);
        text.size(lcd, buffer, x + 30, y + 240, 40, 0xf2652200);
        text.size(lcd, "元/份", x + 70, y + 240, 40, 0xf2652200);
        text.size(lcd, p->data.empty ? "已售罄" : "未售罄", x + 30, y + 280, 40, 0xf2652200);

        Button button(x + 70, y + 320, 60, 50, 0x130c0e00);
        button.show(lcd);
        button.destroyButton();

        text.size(lcd, buffer1, x + 80, y + 320, 40, 0xfffffb00);
        x = x + 250;
    }

    // 创建一个按钮
    Button regist0_1(70, 330, 50, 50, 0x130c0e00);
    regist0_1.setEvent(function0_1); // 设置按钮点击事件
    regist0_1.show(lcd);
    text.size(lcd, "-", 82, 330, 50, 0xfffffb00);

    Button regist0_2(173, 330, 50, 50, 0x130c0e00);
    regist0_2.setEvent(function0_2); // 设置按钮点击事件
    regist0_2.show(lcd);
    text.size(lcd, "+", 187, 330, 50, 0xfffffb00);

    Button regist0_3(320, 330, 50, 50, 0x130c0e00);
    regist0_3.setEvent(function0_3); // 设置按钮点击事件
    regist0_3.show(lcd);
    text.size(lcd, "-", 332, 330, 50, 0xfffffb00);

    Button regist0_4(423, 330, 50, 50, 0x130c0e00);
    regist0_4.setEvent(function0_4); // 设置按钮点击事件
    regist0_4.show(lcd);
    text.size(lcd, "+", 437, 330, 50, 0xfffffb00);

    Button regist0_5(570, 330, 50, 50, 0x130c0e00);
    regist0_5.setEvent(function0_5); // 设置按钮点击事件
    regist0_5.show(lcd);
    text.size(lcd, "-", 582, 330, 50, 0xfffffb00);

    Button regist0_6(673, 330, 50, 50, 0x130c0e00);
    regist0_6.setEvent(function0_6); // 设置按钮点击事件
    regist0_6.show(lcd);
    text.size(lcd, "+", 687, 330, 50, 0xfffffb00);

    Button regist0_7(0, 400, 100, 60, 0x130c0e00);
    regist0_7.setEvent(function0_7); // 设置按钮点击事件
    regist0_7.show(lcd);
    text.size(lcd, "退出", 8, 410, 40, 0xfffffb00);

    Button regist0_8(700, 400, 100, 60, 0x130c0e00);
    regist0_8.setEvent(function0_8); // 设置按钮点击事件
    regist0_8.show(lcd);
    text.size(lcd, "结账", 710, 410, 40, 0xfffffb00);

    Button regist0_9(460, 400, 120, 60, 0x130c0e00);
    regist0_9.setEvent(function0_9); // 设置按钮点击事件
    regist0_9.show(lcd);
    text.size(lcd, "下一页", 460, 410, 40, 0xfffffb00);

    Button regist0_10(210, 400, 120, 60, 0x130c0e00);
    regist0_10.setEvent(function0_10); // 设置按钮点击事件
    regist0_10.show(lcd);
    text.size(lcd, "上一页", 210, 410, 40, 0xfffffb00);

    char buffer[20];
    Button page_button(365, 400, 60, 60, 0x130c0e00);
    page_button.show(lcd);
    snprintf(buffer, sizeof(buffer), "%d", page);
    text.size(lcd, buffer, 380, 410, 40, 0xfffffb00);

    buttons0[0] = regist0_1;
    buttons0[1] = regist0_2;
    buttons0[2] = regist0_3;
    buttons0[3] = regist0_4;
    buttons0[4] = regist0_5;
    buttons0[5] = regist0_6;
    buttons0[6] = regist0_7;
    buttons0[7] = regist0_8;
    buttons0[8] = regist0_9;
    buttons0[9] = regist0_10;

    regist0_1.destroyButton();
    regist0_2.destroyButton();
    regist0_3.destroyButton();
    regist0_4.destroyButton();
    regist0_5.destroyButton();
    regist0_6.destroyButton();
    regist0_7.destroyButton();
    regist0_8.destroyButton();
    regist0_9.destroyButton();
    regist0_10.destroyButton();
    page_button.destroyButton();

    // 获取触摸屏数据
    struct TsDevice *ts = createTs();

    while (1)
    {
        struct Point *p = ts->getXY();

        if (page == 1)
        {
            for (int i = 0; i < 9; i++)
            {
                if (pointOnButton(p, buttons0[i])) // 判断p是否点击在按钮上
                {
                    // 按钮被点击，停止广告线程
                    adThreadActive = false;
                    // 按钮被点击后，设置标志位为真，使所有按钮失效
                    buttons0[i].event(lcd); // 执行按钮点击的事件
                }
            }
        }
        else
        {
            for (int i = 0; i < 10; i++)
            {
                if (pointOnButton(p, buttons0[i])) // 判断p是否点击在按钮上
                {
                    // 按钮被点击，停止广告线程
                    adThreadActive = false;
                    // 按钮被点击后，设置标志位为真，使所有按钮失效
                    buttons0[i].event(lcd); // 执行按钮点击的事件
                }
            }
        }
    }
}

// 按键事件0_1(第一个菜减1)
void function0_1(LcdDevice &lcd)
{
    char buffer[20];

    Node<DataType> *p = list.head; // 创建一个指向结点的指针p后面将用这个指针遍历
    for (int i = 0; i < list.length; i++)
    {
        p = p->next;
        if (i == (page - 1) * 3)
            break;
    }

    if (c[(page - 1) * 3] > 0)
    {
        c[(page - 1) * 3]--;
        snprintf(buffer, sizeof(buffer), "%d", c[(page - 1) * 3]);
        Button regist(130, 330, 60, 50, 0x130c0e00);
        regist.show(lcd);
        text.size(lcd, buffer, 130, 330, 40, 0xfffffb00);
        regist.destroyButton();
    }
}

// 按键事件0_2(第一个菜加1)
void function0_2(LcdDevice &lcd)
{
    char buffer[20];
    Node<DataType> *p = list.head; // 创建一个指向结点的指针p后面将用这个指针遍历
    for (int i = 0; i < list.length; i++)
    {
        p = p->next;
        if (i == (page - 1) * 3)
            break;
    }
    if (p->data.empty == 0)
    {
        c[(page - 1) * 3]++;
        snprintf(buffer, sizeof(buffer), "%d", c[(page - 1) * 3]);
        Button regist(130, 330, 60, 50, 0x130c0e00);
        regist.show(lcd);
        text.size(lcd, buffer, 130, 330, 40, 0xfffffb00);
        regist.destroyButton();
    }
}

// 按键事件0_3(第二个菜减1)
void function0_3(LcdDevice &lcd)
{
    char buffer[20];
    Node<DataType> *p = list.head; // 创建一个指向结点的指针p后面将用这个指针遍历
    for (int i = 0; i < list.length; i++)
    {
        p = p->next;
        if (i == (page - 1) * 3 + 1)
            break;
    }
    if (c[(page - 1) * 3 + 1] > 0)
    {
        c[(page - 1) * 3 + 1]--;
        snprintf(buffer, sizeof(buffer), "%d", c[(page - 1) * 3 + 1]);
        Button regist(380, 330, 60, 50, 0x130c0e00);
        regist.show(lcd);
        text.size(lcd, buffer, 380, 330, 40, 0xfffffb00);
        regist.destroyButton();
    }
}

// 按键事件0_4(第二个菜加1)
void function0_4(LcdDevice &lcd)
{
    char buffer[20];
    Node<DataType> *p = list.head; // 创建一个指向结点的指针p后面将用这个指针遍历
    for (int i = 0; i < list.length; i++)
    {
        p = p->next;
        if (i == (page - 1) * 3 + 1)
            break;
    }
    if (p->data.empty == 0)
    {
        c[(page - 1) * 3 + 1]++;
        snprintf(buffer, sizeof(buffer), "%d", c[(page - 1) * 3 + 1]);
        Button regist(380, 330, 60, 50, 0x130c0e00);
        regist.show(lcd);
        text.size(lcd, buffer, 380, 330, 40, 0xfffffb00);
        regist.destroyButton();
    }
}

// 按键事件0_5(第三个菜减1)
void function0_5(LcdDevice &lcd)
{
    char buffer[20];
    Node<DataType> *p = list.head; // 创建一个指向结点的指针p后面将用这个指针遍历
    for (int i = 0; i < list.length; i++)
    {
        p = p->next;
        if (i == (page - 1) * 3 + 2)
            break;
    }
    if (c[(page - 1) * 3 + 2] > 0)
    {
        c[(page - 1) * 3 + 2]--;
        snprintf(buffer, sizeof(buffer), "%d", c[(page - 1) * 3 + 2]);
        Button regist(630, 330, 60, 50, 0x130c0e00);
        regist.show(lcd);
        text.size(lcd, buffer, 630, 330, 40, 0xfffffb00);
        regist.destroyButton();
    }
}

// 按键事件0_6(第三个菜加1)
void function0_6(LcdDevice &lcd)
{
    char buffer[20];
    Node<DataType> *p = list.head; // 创建一个指向结点的指针p后面将用这个指针遍历
    for (int i = 0; i < list.length; i++)
    {
        p = p->next;
        if (i == (page - 1) * 3 + 2)
            break;
    }
    if (p->data.empty == 0)
    {
        c[(page - 1) * 3 + 2]++;
        snprintf(buffer, sizeof(buffer), "%d", c[(page - 1) * 3 + 2]);
        Button regist(630, 330, 60, 50, 0x130c0e00);
        regist.show(lcd);
        text.size(lcd, buffer, 630, 330, 40, 0xfffffb00);
        regist.destroyButton();
    }
}

// 按键事件0_7(返回主界面)
void function0_7(LcdDevice &lcd)
{
    adThreadActive = true;
    // buttons1[0].destroyButton();
    // buttons1[1].destroyButton();
    // buttons1[2].destroyButton();
    // buttons1[3].destroyButton();
    // buttons1[4].destroyButton();
    // buttons1[5].destroyButton();
    // buttons1[6].destroyButton();
    // buttons1[7].destroyButton();
    // buttons1[8].destroyButton();

    // 销毁lcd
    lcd.destroyLcd();
    // 启动LCD
    start_lcd(0);
}

// 按键事件0_8(结账)
void function0_8(LcdDevice &lcd)
{
    buy = true;
    char buffer[20];
    char buffer1[20];
    char buffer2[20];
    int money = 0;
    int h = 40;
    picture("./foodpicture/b6.bmp", lcd, 50, 10, 700, 380);
    picture("./foodpicture/b5.bmp", lcd, 480, 50, 250, 250);
    text.size(lcd, "购买清单如下：", 100, 10, 30, 0xfffffb00);

    int i;
    Node<DataType> *p = list.head; // 创建一个指向结点的指针p后面将用这个指针遍历
    for (i = 0; i < list.length; i++)
    {
        p = p->next;
        money = money + c[i] * (p->data.price);

        if (c[i] != 0)
        {
            snprintf(buffer1, sizeof(buffer1), "%d", c[i]);
            snprintf(buffer2, sizeof(buffer2), "%d", p->data.price);
            text.size(lcd, p->data.name, 100, h, 30, 0xfffffb00);
            text.size(lcd, buffer1, 230, h, 30, 0xfffffb00);
            text.size(lcd, "份", 280, h, 30, 0xfffffb00);
            text.size(lcd, "每份  元", 330, h, 30, 0xfffffb00);
            text.size(lcd, buffer2, 390, h, 30, 0xfffffb00);
            h = h + 30;
        }
    }

    snprintf(buffer, sizeof(buffer), "%d", money);
    text.size(lcd, "您总共消费了   元", 50, 300, 30, 0xfffffb00);
    text.size(lcd, buffer, 230, 300, 30, 0xfffffb00);
    text.size(lcd, "请扫描二维码付款，稍作等待，小食堂马上为您开火", 50, 330, 30, 0xfffffb00);
    text.size(lcd, "技术有限支付功能还未完善，请手动点击退出(^_^)", 50, 360, 30, 0xfffffb00);
}

// 按键事件0_9(下一页)
void function0_9(LcdDevice &lcd)
{
    page++;
    function0(lcd);
}

// 按键事件0_10(上一页)
void function0_10(LcdDevice &lcd)
{
    page--;
    function0(lcd);
}

// 按键事件1(查看订单)
void function1(LcdDevice &lcd)
{
    // 关闭主界面对应按钮
    close_menu();

    int money = 0;
    char buffer[20];
    char buffer1[20];
    char buffer2[20];
    // 背景1
    picture("./foodpicture/b1.bmp", lcd, 0, 0, 800, 480);
    picture("./foodpicture/b7.bmp", lcd, 50, 10, 700, 380);
    picture("./foodpicture/b8.bmp", lcd, 555, 270, 100, 70);

    int i = 0, x = 150, y = 60;
    Node<DataType> *p = list.head; // 创建一个指向结点的指针p后面将用这个指针遍历
    for (i; i < list.length; i++)
    {
        p = p->next;
        money = money + c[i] * (p->data.price);
        if (c[i] != 0)
        {
            snprintf(buffer, sizeof(buffer), "%d", p->data.price);
            snprintf(buffer1, sizeof(buffer1), "%d", c[i]);

            text.size(lcd, p->data.name, x, y, 30, 0xf2652200);
            // text.size(lcd, p->data.taste, x + 120, y, 30, 0xf2652200);
            text.size(lcd, buffer, x + 200, y, 30, 0xf2652200);
            text.size(lcd, "元/份", x + 230, y, 30, 0xf2652200);
            text.size(lcd, buffer1, x + 320, y, 30, 0xf2652200);
            text.size(lcd, "份", x + 360, y, 30, 0xf2652200);
            y = y + 30;
        }
    }
    snprintf(buffer2, sizeof(buffer2), "%d", money);
    text.size(lcd, "合计是   元", 150, 325, 30, 0xf2652200);
    text.size(lcd, buffer2, 240, 325, 30, 0xf2652200);

    if (buy)
    {
        text.size(lcd, "已结账", 350, 325, 30, 0xf2652200);
    }
    else
    {
        text.size(lcd, "未结账", 350, 325, 30, 0xf2652200);
    }

    Button regist1_1(0, 400, 100, 60, 0x130c0e00);
    regist1_1.setEvent(function1_1); // 设置按钮点击事件
    regist1_1.show(lcd);
    text.size(lcd, "退出", 8, 410, 40, 0xfffffb00);

    Button regist1_2(300, 400, 180, 60, 0x130c0e00);
    regist1_2.setEvent(function1_2); // 设置按钮点击事件
    regist1_2.show(lcd);
    text.size(lcd, "确认打印", 308, 410, 40, 0xfffffb00);

    buttons1[0] = regist1_1;
    buttons1[1] = regist1_2;
    regist1_1.destroyButton();
    regist1_2.destroyButton();

    // 获取触摸屏数据
    struct TsDevice *ts = createTs();

    while (1)
    {
        struct Point *p = ts->getXY();
        // 判断是否点击了按钮
        for (int i = 0; i < 2; i++)
        {
            if (pointOnButton(p, buttons1[i])) // 判断p是否点击在按钮上
            {

                buttons1[i].event(lcd); // 执行按钮点击的事件
            }
        }
    }
}

// 退出订单界面
void function1_1(LcdDevice &lcd)
{
    // 广告标志位
    adThreadActive = true;
    // buttons1[0].destroyButton();
    // buttons1[1].destroyButton();
    //  销毁lcd
    lcd.destroyLcd();
    // 启动LCD
    start_lcd(0);
}

// 确定打印订单
void function1_2(LcdDevice &lcd)
{

    if (!buy)
    {
        Button buyp1(150, 50, 390, 305, 0xfffffb00);
        buyp1.show(lcd);
        buyp1.destroyButton();
        picture("./foodpicture/b11.bmp", lcd, 540, 90, 100, 100);
        text.size(lcd, "请结账后再打印订单", 150, 50, 40, 0xf2652200);
        return;
    }

    Button buyp2(150, 50, 390, 305, 0xfffffb00);
    buyp2.show(lcd);
    buyp2.destroyButton();
    picture("./foodpicture/b12.bmp", lcd, 540, 90, 100, 100);
    text.size(lcd, "订单打印成功！", 150, 50, 40, 0xf2652200);

    int money = 0, i = 0;
    char buffer[20];
    char buffer1[20];
    char buffer2[20];

    // fputs("您的订单为：\n", file);
    //  打开文件
    std::ofstream file("./orders.txt");
    if (!file.is_open())
    {
        std::cerr << "Failed to open file" << std::endl;
        return;
    }

    file << "您的订单为：" << endl;
    Node<DataType> *p = list.head; // 创建一个指向结点的指针p后面将用这个指针遍历
    for (i; i < list.length; i++)
    {
        p = p->next;
        money = money + c[i] * (p->data.price);
        if (c[i] != 0)
        {
            snprintf(buffer1, sizeof(buffer1), "%d", c[i]);
            snprintf(buffer2, sizeof(buffer2), "%d", p->data.price);

            cout << buffer1 << endl;
            cout << buffer2 << endl;
            p->data.number = p->data.number + c[i];
            cout << p->data.number << endl;
            file << p->data.name << " " << buffer2 << "元/份" << " " << buffer1 << "份" << std::endl;
        }
    }

    snprintf(buffer, sizeof(buffer), "%d", money);
    file << "共：" << buffer << "元" << std::endl;

    file.close();

    Client::send_list();

    int n = 0;
    for (n; n < 30; n++)
    {
        c[n] = 0;
    }

    // 恢复未购买状态
    buy = false;
    readFromFile();
}

// 读取订单文件
void readFromFile()
{
    // 打开文件
    std::ifstream file("./orders.txt");
    if (!file.is_open())
    {
        std::cerr << "Failed to open file" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::cout << line << std::endl;
    }

    // 关闭文件
    file.close();
}

// 按键事件2(餐厅介绍)
void function2(LcdDevice &lcd)
{
    // 背景音乐进程
    // system("madplay  ./foodpicture/wlcc.mp3 -r &");
    // 关闭主界面对应按钮
    close_menu();
    picture("./foodpicture/b1.bmp", lcd, 0, 0, 800, 480);
    picture("./foodpicture/b9.bmp", lcd, 450, 320, 230, 120);
    picture("./foodpicture/b10.bmp", lcd, 150, 320, 230, 120);

    // 显示
    MyFont text(800, 480);
    text.size(lcd, "以下内容纯属虚构，请勿代入现实：", 50, 20, 30, 0xfffffb00);
    text.size(lcd, "中华小食堂起源于西南四川省地区驰名的菊下楼。", 50, 50, 30, 0xfffffb00);
    text.size(lcd, "1967年,中国四川省的年轻厨师刘星星在母亲刘阿贝", 50, 80, 30, 0xfffffb00);
    text.size(lcd, "(菊下楼主厨)的推荐下到广州阳泉酒家修行学习厨艺,", 50, 110, 30, 0xfffffb00);
    text.size(lcd, "获得特级厨师资格,结交了七星刀雷恩、钢棍蟹师傅等", 50, 140, 30, 0xfffffb00);
    text.size(lcd, "一众有志于厨艺的朋友,于1973年共同创立了中华小", 50, 170, 30, 0xfffffb00);
    text.size(lcd, "食堂与外国对抗。自成立以来中华小食堂一直是对抗", 50, 200, 30, 0xfffffb00);
    text.size(lcd, "外国美食界的主力军，获得了人民大众的广泛好评。", 50, 230, 30, 0xfffffb00);
    Button button(10, 400, 100, 60, 0x130c0e00);
    button.setEvent(function2_1); // 设置按钮点击事件
    button.show(lcd);
    text.size(lcd, "退出", 8, 410, 40, 0xfffffb00);

    // 获取触摸屏数据
    struct TsDevice *ts = createTs();
    while (1)
    {
        struct Point *p = ts->getXY();
        // 判断是否点击了按钮
        if (pointOnButton(p, button)) // 判断p是否点击在按钮上
        {
            // 用来关闭背景音乐进程
            // system("killall -9 madplay");
            button.event(lcd); // 执行按钮点击的事件
        }
    }
}

// 退出餐厅介绍
void function2_1(LcdDevice &lcd)
{
    // 广告标志位
    adThreadActive = true;
    // 销毁lcd
    lcd.destroyLcd();
    // 启动LCD
    start_lcd(0);
}

// 按键事件3(VIP服务)
void function3(LcdDevice &lcd)
{
    close_menu();

    picture("./foodpicture/b1.bmp", lcd, 0, 0, 800, 480);
    picture("./foodpicture/b4.bmp", lcd, 60, 10, 236, 354);
    picture("./foodpicture/b5.bmp", lcd, 500, 10, 220, 220);
    text.size(lcd, "现在扫描屏幕上方二维码", 430, 280, 30, 0xfffffb00);
    text.size(lcd, "并办理VIP会员", 430, 310, 30, 0xfffffb00);
    text.size(lcd, "即可领取周年纪念品", 430, 340, 30, 0xfffffb00);
    text.size(lcd, "特级厨师围裙", 90, 370, 30, 0xf2652200);
    text.size(lcd, "特级厨师围裙", 430, 370, 30, 0xfffffb00);

    Button button(0, 400, 100, 60, 0x130c0e00);
    button.setEvent(function3_1); // 设置按钮点击事件
    button.show(lcd);
    text.size(lcd, "退出", 8, 410, 40, 0xfffffb00);

    // 获取触摸屏数据
    struct TsDevice *ts = createTs();
    while (1)
    {
        struct Point *p = ts->getXY();
        // 判断是否点击了按钮
        if (pointOnButton(p, button)) // 判断p是否点击在按钮上
        {
            // 用来关闭背景音乐进程
            // system("killall -9 madplay");
            button.event(lcd); // 执行按钮点击的事件
        }
    }
}

// 退出VIP服务
void function3_1(LcdDevice &lcd)
{
    // 广告标志位
    adThreadActive = true;

    // 销毁lcd
    lcd.destroyLcd();
    // 启动LCD
    start_lcd(0);
}

// 启动LCD
void *start_lcd(void *arg)
{
    // 创建LCD
    LcdDevice lcd;
    // 清屏
    lcd.clear(0xffffff);
    // 背景1
    picture("./foodpicture/b1.bmp", lcd, 0, 0, 800, 480);

    // 显示
    // MyFont text(800, 480);

    // 文字1
    text.size(lcd, "中", 20, 100, 60, 0xed194100);
    text.size(lcd, "华", 20, 170, 60, 0xed194100);
    text.size(lcd, "小", 20, 240, 60, 0xed194100);
    text.size(lcd, "食", 20, 310, 60, 0xed194100);
    text.size(lcd, "堂", 20, 380, 60, 0xed194100);

    // 创建一个按钮
    Button regist(110, 80, 580, 380, 0xafdfe400);
    regist.show(lcd);
    // 背景2
    picture("./foodpicture/b2.bmp", lcd, 110, 80, 250, 380);

    Button regist0(360, 80, 330, 100, 0xf2652200);
    regist0.setText("开始点餐");
    regist0.setEvent(function0); // 设置按钮点击事件
    regist0.show(lcd);
    buttons[0] = regist0;

    Button regist1(360, 180, 330, 100, 0x33a3dc00);
    regist1.setText("查看订单");
    regist1.setEvent(function1); // 设置按钮点击事件
    regist1.show(lcd);
    buttons[1] = regist1;

    Button regist2(360, 280, 165, 100, 0x65c29400);
    regist2.setText("餐厅介绍");
    regist2.setEvent(function2); // 设置按钮点击事件
    regist2.show(lcd);
    buttons[2] = regist2;

    Button regist3(525, 280, 165, 100, 0xf391a900);
    regist3.setText("VIP服务");
    regist3.setEvent(function3); // 设置按钮点击事件
    regist3.show(lcd);
    buttons[3] = regist3;

    text.size(lcd, "订餐电话：23333333", 380, 410, 30, 0xed194100);

    // 多线程处理
    pthread_t th1;

    // 线程参数(混合类)
    Mix m1(regist, lcd);

    pthread_create(&th1, NULL, ad, &lcd);

    m1.aj(0);
    pthread_join(th1, NULL);
    std::cout << "线程2已回收" << endl;

    // 销毁lcd
    // lcd.destroyLcd();
    return 0;
}