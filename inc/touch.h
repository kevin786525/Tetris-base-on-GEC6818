#ifndef _TOUCH_H_
#define _TOUCH_H_

#include "commonheader.h"
#include "my_types.h"
#include "head4animation.h"



//函数名:TC_Init()
//功  能:触摸屏初始化文件
void TC_Init(void);

//函数名:touch_fun()
//功  能:触摸屏
void * touch_fun(void * arg);


void slide_right2left();
void slide_left2right();




#endif