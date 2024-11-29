#include "lcddevice.hpp"
#include "client.hpp"

// 构造函数实现
LcdDevice::LcdDevice()
{
    initialize();
}

/*
// 析构函数实现
LcdDevice::~LcdDevice()
{
    destroyLcd();
}*/

// 初始化LCD设备
void LcdDevice::initialize()
{
    // 打开设备
    fd = open("/dev/fb0", O_RDWR);
    if (fd < 0)
    {
        perror("open lcd");
        throw std::runtime_error("Failed to open LCD device");
    }

    // 从设备驱动中获取LCD信息
    struct fb_var_screeninfo info;
    int ret = ioctl(fd, FBIOGET_VSCREENINFO, &info);
    if (ret < 0)
    {
        perror("get info");
        close(fd);
        throw std::runtime_error("Failed to get LCD info");
    }

#ifdef UBUNTU
    // 初始化
    width = info.xres_virtual;
    height = info.yres_virtual;
    pixel = info.bits_per_pixel / 8;
#else
    // 初始化
    width = info.xres;
    height = info.yres;
    pixel = info.bits_per_pixel / 8;
#endif

    // 映射内存
    mptr = (unsigned int *)mmap(NULL, width * height * pixel, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mptr == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        throw std::runtime_error("Failed to mmap LCD memory");
    }
}

// 释放资源
void LcdDevice::destroyLcd()
{
    // 释放映射
    munmap(mptr, width * height * pixel);
    // 关闭文件描述符
    close(fd);
}

// 清屏
void LcdDevice::clear(unsigned int color)
{
    unsigned int *p = mptr; // 保存LCD里面映射指针不能动
    // 写像素点
    for (int j = 0; j < height; j++)
    { // 写480行
        for (int i = 0; i < width; i++)
        {                             // 写一行
            p[i + j * width] = color; // 写一个像素点
        }
    }
}

// 在LCD上显示BMP图片数据
void LcdDevice::draw_rgb(int x, int y, Bmp &bmp)
{
    unsigned int *p = mptr + y * width + x;
    unsigned char *rgb = bmp.data;

    int draw_w = ((x + bmp.width) > width) ? (width - x) : bmp.width;
    int draw_h = ((y + bmp.height) > height) ? (height - y) : bmp.height;
    for (int i = 0; i < draw_h; i++)
    {
        for (int j = 0; j < draw_w; j++)
        {
            memcpy(p + j, rgb + j * bmp.pixel, bmp.pixel);
        }
        p += width;
        rgb += bmp.width * bmp.pixel;
    }
}


