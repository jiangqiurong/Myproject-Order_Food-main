#ifndef _BMP_HPP_
#define _BMP_HPP_

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

// BMP文件头结构体定义
struct bitmap_header
{
    int16_t type;          // 文件类型，必须是0x4D42，即字符'BM'
    int32_t filesize;      // 文件大小，以字节为单位
    int16_t reserved1;     // 保留字段，必须为0
    int16_t reserved2;     // 保留字段，必须为0
    int32_t offbits;       // 从文件头到像素数据的偏移量
    int32_t size;          // 本结构体的大小
    int32_t width;         // 图像宽度，以像素为单位
    int32_t height;        // 图像高度，以像素为单位
    int16_t planes;        // 目标设备的平面数，必须为1
    int16_t bit_count;     // 每个像素的位数（色深）
    int32_t compression;   // 压缩类型，0表示不压缩
    int32_t size_img;      // 图像数据大小，必须是4的倍数
    int32_t X_pel;         // 水平分辨率，像素/米
    int32_t Y_pel;         // 垂直分辨率，像素/米
    int32_t clrused;       // 使用的颜色数，0表示使用所有颜色
    int32_t clrImportant;  // 重要颜色数，0表示所有颜色都重要
} __attribute__((packed)); // 确保结构体没有填充字节

// Bmp类定义
class Bmp
{
public:
    int width;           // 保存图片宽度
    int height;          // 保存图片高度
    int pixel;           // 保存一个像素占用的字节数
    unsigned char *data; // 保存RGB数据（需要分配堆空间）
    
    // 构造函数
    Bmp();                             // 默认构造函数
    Bmp(const char *bmpPath);          // 构造函数，传入BMP文件路径

    void reversal();                   // 反转图片数据
    void zoomBmp(int w, int h);           // 缩放图片数据到指定尺寸
    void loadBmp(const char *bmpPath); // 加载BMP文件
    void destroyBmp();                  // 销毁BMP对象
};



#endif // _BMP_HPP_
