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

//=======================申明全局变量=======================//
extern int fd_lcd;
extern int fd_tc;

extern int pos_x;
extern int pos_y;
//=======================申明全局变量=======================//


#define DEBUG 0

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
    int collect_cnt = collect_file(argv[1], my_list);
    printf("共搜索到%d个文件!\n", collect_cnt);

    //显示链表
    show_list(my_list);
    P_node tmp = search_2_list(my_list, "goku");
    unsigned long (* buf)[WIDTH] = calloc(1, SCREEN_SIZE * 4);
    printf("申请内存成功\n");
    // buf = (unsigned long (*)[WIDTH])lcd_pos_size_pixel(tmp, 0, 0, 800, 480);
    get_image(tmp, buf);
    printf("得到图片信息\n");
    left2right_in(fd_lcd, buf);

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
    pthread_create(&tc_tid, NULL, touch_fun, NULL);
    //将触摸屏以及状态栏的线程分离
    if( pthread_detach(tc_tid) != 0){
        perror("触摸屏线程分离失败:");
    }   

    /* //创建线程运行状态栏函数
    pthread_t time_tid;
    pthread_create(&time_tid, NULL, flash_time, NULL);
    if( pthread_detach(time_tid) != 0){
        perror("时间状态栏线程分离失败:");
    }
     */
    //进入屏幕操作系统
    my_system(my_list);
#endif
    //关闭相关资源
    munmap(map ,HEIGHT*WIDTH*4*3);
    close(fd_lcd);
    close(fd_tc);
}