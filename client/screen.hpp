#ifndef __SCREEN_HPP__
#define __SCREEN_HPP__

#include <iostream>
#include <pthread.h>
#include "lcddevice.hpp"
#include "font.hpp"
#include "button.hpp"
#include "tsevent.hpp"

// 判断点是否在按钮上
bool pointOnButton(struct Point *p, Button &button);

// 背景
void picture(const char *bmp, LcdDevice &lcd, int x, int y, int w, int h);
// 广告
void *ad(void *args);

// 关闭主界面对应按钮
void close_menu();

// 按键事件0
void function0(LcdDevice &lcd);
void function0_1(LcdDevice &lcd);
void function0_2(LcdDevice &lcd);
void function0_3(LcdDevice &lcd);
void function0_4(LcdDevice &lcd);
void function0_5(LcdDevice &lcd);
void function0_6(LcdDevice &lcd);
void function0_7(LcdDevice &lcd);
void function0_8(LcdDevice &lcd);
void function0_9(LcdDevice &lcd);
void function0_10(LcdDevice &lcd);


//按键事件1
void function1(LcdDevice &lcd);
//对应按键事件1的退出功能
void function1_1(LcdDevice &lcd);
//对应按键事件1的打印功能
void function1_2(LcdDevice &lcd);
// 读取订单文件
void readFromFile();

//按键事件2
void function2(LcdDevice &lcd);
// 对应按键事件2的退出功能
void function2_1(LcdDevice &lcd);
//按键事件3
void function3(LcdDevice &lcd);
// 对应按键事件3的退出功能
void function3_1(LcdDevice &lcd);

// 启动LCD
void* start_lcd(void* args);

#endif