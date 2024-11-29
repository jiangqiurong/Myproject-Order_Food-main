#include "tsevent.hpp"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <string.h>
#include <stdlib.h>
static struct TsDevice *ts = NULL;

static struct Point *getXY()
{
    //2.读取数据
    struct input_event inevt;
    static struct Point point;
    memset(&point, 0, sizeof(struct Point));
    while(1)
    {
        if(point.x != 0 && point.y != 0) break;

        int ret = read(ts->fd, &inevt, sizeof(inevt));
        if(ret < 0) break;
        //判断读取的是否是x坐标
        if(inevt.type == EV_ABS && inevt.code==ABS_X)
        {
            point.x = inevt.value;
            continue;
        }
        //判断读取的是否是y坐标
        if(inevt.type == EV_ABS && inevt.code==ABS_Y)
        {
            point.y = inevt.value;
        }  
    }
    //触摸屏与lcd尺寸换算
    point.x = point.x*800/1024;
    point.y = point.y*480/600;
    return &point;
}

static void destroyTs()
{
    if(ts != NULL)
    close(ts->fd);
    free(ts);
    ts = NULL;
}

struct TsDevice *createTs(){
    //判断触摸屏ts是否已经被初始化
    if(ts!=NULL) return ts;
    //ts = malloc(sizeof(struct TsDevice));
    ts = static_cast<TsDevice *>(malloc(sizeof(TsDevice)));
    if(ts == NULL) return NULL;
    ts->fd = open("/dev/input/event0",O_RDWR);
    if(ts->fd < 0) 
    {
        perror("ts open");
        free(ts);
        return NULL;
    }
    ts->getXY = getXY;
    ts->destroyTs = destroyTs;
    return ts;
}

