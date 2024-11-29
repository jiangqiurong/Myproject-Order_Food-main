#ifndef _TSEVENT_HPP
#define _TSEVENT_HPP

#include<stdbool.h>
#include <iostream>

struct Point {
    int x, y;
};
struct TsDevice{  //触摸屏1024*600，但是lcd是800*480
    int fd;//文件描述符
    struct Point*(*getXY)();
    void (*destroyTs)();
};

struct TsDevice *createTs();




#endif//_TSEVENT_H