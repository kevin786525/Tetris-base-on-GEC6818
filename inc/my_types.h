#ifndef _MY_TYPES_H_
#define _MY_TYPES_H_

#include "Kernel_List.h"
// typedef char * data_type;


typedef struct my_type{
    //存储文件名
    char name[257];
    //存储文件类型
    char type;
}data_type, *P_Data;

typedef struct node{

    //数据域
    data_type Data;

    //指针域
    struct list_head ptr;
}Node , *P_node;


struct pos{
    unsigned int x;
    unsigned int y;
};


//定义触摸状态位的结构体，使触摸变成阻塞型
struct touch_state{
    struct pos start_pos;
    struct pos end_pos;
    bool x_ready;
    bool y_ready;
    bool r_slide;
    bool l_slide;
};




#endif