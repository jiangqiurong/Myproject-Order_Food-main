#ifndef __FONT_HPP__
#define __FONT_HPP__
#include <iostream>
#include "lcddevice.hpp"

extern "C"
{
#include "font.h"
}

using namespace std;

class MyFont
{
private:
    font *f;
    bitmap *bitm;

public:
    MyFont(int w = 800, int h = 480, unsigned int color = 0xfffffffb, string ttf = "./simfang.ttf")
    {
        // 打开字体	 可以根据自己的喜好从Windows中选择字体注意必须是ttf格式
        f = fontLoad(const_cast<char *>(ttf.c_str()));

        // 创建一个画板（点阵图）
        bitm = createBitmapWithInit(w, h, 4, color); // 也可使用createBitmapWithInit函数，改变画板颜色
    }

    ~MyFont()
    {
        // 释放点阵空间
        destroyBitmap(bitm);
        // 释放字库
        fontUnload(f);
    }

    /*
    //将字体写到点阵图上
    void FontPrint(int x, int y, unsigned int color , string msg)
    {
        fontPrint(f,bm , x , y , const_cast<char *>( msg.c_str()) , color,0) ;
    }

    void Show_font_to_lcd( struct LcdDevice* lcd , int x ,int y )
    {
        show_font_to_lcd(lcd->mptr,x , y ,bm);
    }*/

    // 绘制文字
    void size(LcdDevice &lcd, string msg, int x, int y, int size, unsigned int color)
    {
        // 设置字体大小
        fontSetSize(f, size);                // 一个汉字占用32行32列
        unsigned char buffer[800 * 480 * 4]; // 提取屏幕的背景暂存
        unsigned int *p = lcd.mptr;
        unsigned char *q = bitm->map;
        for (int i = 0; i < 480; i++)
        {
            memcpy(q, p, 800 * 4);
            q += 800 * 4;
            p += lcd.width;
        }

        // 把汉字输出到点阵
        fontPrint(f, bitm, x, y, const_cast<char *>(msg.c_str()), color, 0); // #RGBA
        // 把汉字点阵区域绘制在lcd上
        // struct Bmp bm = {bitm->width, bitm->height, bitm->byteperpixel, bitm->map};
        Bmp bm;
        bm.width = bitm->width;
        bm.height = bitm->height;
        bm.pixel = bitm->byteperpixel;
        bm.data = bitm->map;
        lcd.draw_rgb(0, 0, bm);
    }
};

#endif