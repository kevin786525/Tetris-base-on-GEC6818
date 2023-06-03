////////////////////////////////////////////////////////////////////////
//                                                                    //
//      时间:2022-5-27                                                //
//      作者:lee                                                      //
//      内容:实现第一阶段项目的主函数                                 //
//                                                                    //
//                                                                    //
////////////////////////////////////////////////////////////////////////

#include "Main_Function.h"
#include "list.h"
#include "my_types.h"
#include "head4animation.h"
#include "touch.h"
#include "lcd.h"
#include "weather.h"

//=======================申明全局变量=======================//
extern int fd_lcd;
extern int fd_tc;

extern int pos_x;
extern int pos_y;


extern pthread_mutex_t lock;
//=======================申明全局变量=======================//


#define DEBUG 1

void main(int argc, char *argv[]){

    //检查外部参数
    if(argc != 2){
        perror("外部参数错误,请按格式:./<可执行文件>  <源文件/目录>");
        exit(0);
    }


    //初始化相关文件
    LCD_Init();
    TC_Init();

    //申请映射内存
    int * map = Map_Init();

    //创建双向链表
    P_node my_list = List_Init();


#if DEBUG
    //进入目录内收集相关文件
    int collect_cnt = collect_file(argv[1], my_list);
    printf("共搜索到%d个文件!\n", collect_cnt);

    //显示链表
    show_list(my_list);

    // bmp_list = handle_list(bmp_list, my_list);
    //创建线程运行获取坐标函数
    pthread_t tc_tid;
    pthread_create(&tc_tid, NULL, touch_fun, NULL);
    //将触摸屏以及状态栏的线程分离
    if( pthread_detach(tc_tid) != 0){
        perror("触摸屏线程分离失败:");
    } 

    play_tetris(my_list);

    //进入屏幕操作系统
    // my_system(my_list);

#endif



#if !DEBUG 
    //显示lcd设备信息
    show_lcd_info();

    //进入目录内收集相关文件
    int collect_cnt = collect_file(argv[1], my_list);
    printf("共搜索到%d个文件!\n", collect_cnt);

    //显示链表
    show_list(my_list);

    //创建线程运行获取坐标函数
    pthread_t tc_tid;
    
    pthread_attr_t tc_attr;
    pthread_attr_init(&tc_attr);                                    //初始化线程属性变量
    pthread_attr_setinheritsched(&tc_attr, PTHREAD_EXPLICIT_SCHED); //设置为自己设置调度策略
    pthread_attr_setschedpolicy(&tc_attr, SCHED_FIFO);              //设置线程的调度策略为FIFO的先进先出的排队方式
    
    struct sched_param tc_param;
    memset(&tc_param, 0, sizeof(struct sched_param));               //设置线程的优先级的静态优先级
    tc_param.sched_priority = 12;                                   //设置线程的静态优先级为12
    pthread_attr_setschedparam(&tc_attr, &tc_param);
    pthread_create(&tc_tid, &tc_attr, touch_fun, NULL);
    //将触摸屏以及状态栏的线程分离
    if( pthread_detach(tc_tid) != 0){
        perror("触摸屏线程分离失败:");
    }   

    // //创建线程运行状态栏函数
    // pthread_t time_tid;
    // pthread_create(&time_tid, NULL, flash_time, NULL);
    // if( pthread_detach(time_tid) != 0){
    //     perror("时间状态栏线程分离失败:");
    // }
    
    //进入屏幕操作系统
    my_system(my_list);
#endif
    //关闭相关资源
    munmap(map ,HEIGHT*WIDTH*4);
    close(fd_lcd);
    close(fd_tc);
}