#ifndef _WEATHER_H_
#define _WEATHER_H_

#include "commonheader.h"
#include "font.h"
#include "touch.h"
#include "bmp.h"


//功  能:滚动刷新天气
void *flash_weather(void * arg);


//功  能:在状态栏刷新时间,以及滚动显示天气信息
void * flash_time(void * arg);

//改变字体的背景
void font_background(bitmap * bm, int x, int y);


#endif