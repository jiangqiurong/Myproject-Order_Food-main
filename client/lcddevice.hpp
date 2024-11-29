#ifndef _LCDDEVICE_HPP
#define _LCDDEVICE_HPP

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <stdbool.h>
#include "bmp.hpp"


// LcdDevice类定义
class LcdDevice
{
public:
    int fd;             // LCD文件描述符
    int width;          // 宽
    int height;         // 高
    int pixel;          // 一个像素占用的字节数
    unsigned int *mptr; // 保存映射空间的首地址

    LcdDevice();                           // 构造函数
    //~LcdDevice();                          // 析构函数
    void clear(unsigned int color);        // 清屏
    void draw_rgb(int x, int y, Bmp &bmp); // 在LCD上显示BMP图片数据
    void initialize(); // 初始化LCD设备
    void destroyLcd(); // 释放资源
};


#endif // _LCDDEVICE_HPP
