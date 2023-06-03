////////////////////////////////////////////////////////////////////////
//                                                                    //
//      时间:2022-5-27                                                //
//      作者:lee                                                      //
//      内容:实现第一阶段项目的主要功能函数声明                         //
//                                                                    //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#ifndef _MAIN_FUNCTION_H_
#define _MAIN_FUNCTION_H_

#include "commonheader.h"
#include "list.h"
#include "font.h"
#include "my_types.h"
#include "head4animation.h"
#include "bmp.h"
#include "Kernel_List.h"
#include "lcd.h"
#include "tetris.h"




//函数名:Map_Init()
//功  能:申请映射内存
int * Map_Init(); 

//函数名:collect_file()
//功  能:收集目录内相关(.mp3/bmp(拓展.jpg)文件)
//       并且存入相关链表中
unsigned int collect_file(const char * Path, P_node my_list);

//函数名:IS_BMPfile()
//功  能:判断是否为bmp图片
bool IS_BMPfile(char * name);

//函数名:IS_MP3file()
//功  能:判断是否为mp3文件
bool IS_MP3file(char * name);

//函数名:IS_AVI_file()
//功  能:判断是否为avi文件
bool IS_AVI_file(char * name);


//函数名:play_music();
//功  能:播放音乐
void play_music(P_node my_list);

//函数名:play_video()
//功  能:播放视频
void play_video(P_node my_list);

//函数名:my_system()
//功  能:操作系统
void my_system(P_node my_list);

//功  能:寻找上一首mp3的节点
P_node last_song(P_node pos, P_node my_list);
//功  能:寻找下一首mp3的节点
P_node next_song(P_node pos, P_node my_list);



//功  能:开机的界面锁
bool project_lock();

//功  能:获取键盘上下左右键
void get_direction();


//功  能: 滑动更改图片显示(未完成)
//参  数: @mod: [1] 将b区图片内存拷贝到a区，实现下一张
//              [2] 将c区图片内存拷贝到a区，实现上一张
void slide2change(unsigned int mod);

//功  能: 判断上滑退出
bool upslide2exit(void);

#endif