#include "button.hpp"
#include "font.hpp"

// 构造函数实现
Button::Button(int x, int y, int w, int h, unsigned int color)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->backpicture = 0;    // 背景图片
    this->backcolor = color;  // 背景颜色
    this->color = 0xfffffb00; // 字体颜色

    // 添加字库
    char *filename = (char *)"./simfang.ttf";
    this->f = fontLoad(filename);

    // 设置字体大小
    fontSetSize(this->f, this->w / 6);
    // 创建字体点阵区域
    this->bitm = createBitmapWithInit(w, h, 4, this->backcolor);
}


/*
Button::~Button()
{
    if (this->backpicture != 0)
    {
        delete this->backpicture; // 释放背景图片
    }
    destroyBitmap(this->bitm); // 释放汉字点阵区域
    fontUnload(this->f);       // 释放字库
}*/

// 显示按钮
void Button::show(LcdDevice &lcd)
{
    // 保存绘制的首地址
    unsigned int *p = lcd.mptr + this->x + this->y * lcd.width;
    unsigned char *q = this->bitm->map;
    for (int i = 0; i < this->h; i++)
    {
        memcpy(p, q, this->w * this->bitm->byteperpixel);
        p += lcd.width;
        q += this->w * this->bitm->byteperpixel;
    }
}

// 设置文本
void Button::setText(const char *text)
{
    // 绘制文字
    fontPrint(this->f, this->bitm, (this->w) / 6, (this->h) / 4, (char *)text, this->color, 0);
}

// 设置事件
void Button::setEvent(void (*event)(LcdDevice &))
{
    this->event = event;
}


void Button:: destroyButton()
{
    if (this->backpicture != 0)
    {
        delete this->backpicture; // 释放背景图片
    }
    destroyBitmap(this->bitm); // 释放汉字点阵区域
    fontUnload(this->f);       // 释放字库
}