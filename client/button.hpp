#ifndef _BUTTON_HPP_
#define _BUTTON_HPP_

#include <iostream>
#include "lcddevice.hpp"
#include "bmp.hpp"
#include "tsevent.hpp"

extern "C"
{
#include "font.h"
}

// Button类定义
class Button
{
public:
    int w, h;                   // 按钮宽度和高度
    int x, y;                   // 按钮位置
    char text[32];              // 按钮文字
    unsigned int backcolor;     // 背景颜色
    unsigned int color;         // 字体颜色
    Bmp *backpicture;           // 存储按钮背景图片
    void (*event)(LcdDevice &); // 按钮点击调用的函数
    font *f;                    // 字库
    bitmap *bitm;               // 绘制文字点阵区域

    Button(int x=0, int y=0, int w=0, int h=0, unsigned int color=0); // 构造函数
    //~Button();                                              // 析构函数
    void show(LcdDevice &lcd);                              // 显示按钮
    void setText(const char *text);                         // 设置文本
    void setEvent(void (*event)(LcdDevice &));              // 设置事件
    void *aj(void *args);                                   // 按键线程参数
    void destroyButton();                                   // 销毁按钮
};


#endif // _BUTTON_HPP_
