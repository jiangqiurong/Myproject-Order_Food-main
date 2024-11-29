#include "bmp.hpp"

//默认构造函数实现
Bmp::Bmp()
{
    width = 0;
    height = 0;
    pixel = 0;
    data = NULL;
}

// 构造函数实现
Bmp::Bmp(const char *bmpPath)
{
    loadBmp(bmpPath); // 调用加载BMP文件的函数
}

// 销毁函数实现
void Bmp::destroyBmp()
{
    if (data != NULL)
    {
        free(data); // 释放分配的内存
    }
}

// 加载BMP文件的函数实现
void Bmp::loadBmp(const char *bmpPath)
{
    std::ifstream file(bmpPath, std::ios::binary); // 以二进制模式打开文件
    if (!file)
    {
        std::cerr << "Failed to open file: " << bmpPath << std::endl;
        return;
    }

    bitmap_header header;
    file.read(reinterpret_cast<char *>(&header), sizeof(header)); // 读取文件头
    if (file.gcount() != sizeof(header))
    {
        std::cerr << "Failed to read BMP header" << std::endl;
        return;
    }

    width = header.width;         // 获取图像宽度
    height = header.height;       // 获取图像高度
    pixel = header.bit_count / 8; // 计算每个像素占用的字节数

    int linebyte = width * pixel; // 计算一行像素占用的字节数
    int b = linebyte % 4;         // 计算一行像素占用的字节数是否是4的倍数
    if (b != 0)
        b = 4 - b;               // 如果不是4的倍数，计算需要补齐的字节数
    int linefile = linebyte + b; // 计算文件中一行占用的字节数

    data = static_cast<unsigned char *>(malloc(linebyte * height)); // 分配内存存储图像数据
    if (!data)
    {
        std::cerr << "Failed to allocate memory for BMP data" << std::endl;
        return;
    }

    for (int i = 0; i < height; i++)
    {
        file.read(reinterpret_cast<char *>(data + linebyte * i), linefile); // 读取一行像素数据
    }
    file.close(); // 关闭文件

    reversal(); // 调用反转图片数据的函数
}

// 反转图片数据的函数实现
void Bmp::reversal()
{
    int linebyte = width * pixel;       // 计算一行像素占用的字节数
    unsigned char linebuffer[linebyte]; // 定义一个缓冲区存储一行像素数据

    for (int i = 0; i < height / 2; i++)
    {
        memcpy(linebuffer, data + i * linebyte, linebyte);                         // 暂存上半部分的一行数据
        memcpy(data + i * linebyte, data + (height - i - 1) * linebyte, linebyte); // 将下半部分的一行数据复制到上半部分
        memcpy(data + (height - i - 1) * linebyte, linebuffer, linebyte);          // 将暂存的上半部分数据复制到下半部分
    }
}

// 缩放图片数据的函数实现
void Bmp::zoomBmp(int w, int h)
{
    unsigned char *dest = static_cast<unsigned char *>(malloc(w * h * pixel)); // 分配内存存储缩放后的图像数据
    int bw = width * 100.0 / w;                                                // 计算宽度缩放比例
    int bh = height * 100.0 / h;                                               // 计算高度缩放比例

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            dest[j * 3 + 0 + i * w * 3] = data[(bw * j / 100) * 3 + (bh * i / 100) * width * 3 + 0]; // 缩放后的R分量
            dest[j * 3 + 1 + i * w * 3] = data[(bw * j / 100) * 3 + (bh * i / 100) * width * 3 + 1]; // 缩放后的G分量
            dest[j * 3 + 2 + i * w * 3] = data[(bw * j / 100) * 3 + (bh * i / 100) * width * 3 + 2]; // 缩放后的B分量
        }
    }

    free(data);  // 释放原图像数据内存
    width = w;   // 更新图像宽度
    height = h;  // 更新图像高度
    data = dest; // 更新图像数据指针
}