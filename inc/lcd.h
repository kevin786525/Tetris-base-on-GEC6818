#ifndef _LCD_H_
#define _LCD_H_

#include "commonheader.h"

//函数名:LCD_Init()
//功  能:初始化lcd屏幕文件
void LCD_Init(void);


//函数名:get_fixinfo()
//功  能:获取LCD设备的固定属性
void get_fixinfo();


//函数名:get_varinfo()
//功  能:获取LCD设备的可变属性
void get_varinfo();

//函数名:show_info()
//功  能:显示屏幕设备的信息
void show_lcd_info();
#endif