#include "touch.h"

#define NONBLOCK 1
#define DEBUG 0

//============================全局变量定义============================//

int fd_tc = -1;

extern int * map;
extern int * reserve_mem;

volatile unsigned long pos_x = 0;
volatile unsigned long pos_y = 0;

// volatile unsigned int in_x = 0;
// volatile unsigned int in_y = 0; // 按下的坐标值
// volatile unsigned int out_x = 0;
// volatile unsigned int out_y = 0; // 松手坐标值
volatile unsigned int pre_x = 0;

volatile unsigned int x_diff = 0;

struct touch_state tc_state;
struct input_event pos_buf;


//============================全局变量定义============================//


//功  能:触摸屏初始化文件
void TC_Init(void){

    fd_tc = open("/dev/input/event0",O_RDWR);
    if(fd_tc < 0){
        perror("打开tc文件失败!");
        exit(0);
    }
}


//功  能:触摸屏(放进线程，一直执行)
void * touch_fun(void * arg){

    tc_state.r_slide = false;
    tc_state.l_slide = false;
    // struct input_event pos_buf;
    memset(&pos_buf, 0, sizeof(pos_buf));

#if NONBLOCK
    // 将触摸屏设置成非阻塞属性
    int status;
    status = fcntl(fd_tc, F_GETFL);    //获取当前status状态字
    status |= O_NONBLOCK;           //加上"非阻塞的状态"
    fcntl(fd_tc, F_SETFL, status);  //将配置好的status设置到触摸屏中
#endif

    //分析结构体数据
    while(1){
        read(fd_tc, &pos_buf, sizeof(pos_buf));
        if(pos_buf.type == EV_ABS){  //若类型是触摸

            if(pos_buf.code == ABS_X){
                pos_x = pos_buf.value * 1.0 *800 / 1024;
            }
            if(pos_buf.code == ABS_Y){
                pos_y = pos_buf.value * 1.0 *480 / 600;
            }
        }

        //判断摁下以及松手
        if(pos_buf.type == EV_KEY && pos_buf.code == BTN_TOUCH && pos_buf.value > 0){      //摁下
            tc_state.start_pos.x = pos_x;
            tc_state.start_pos.y = pos_y;
            tc_state.x_ready = true;
            tc_state.y_ready = true;
        }else if( pos_buf.type == EV_KEY && pos_buf.code == BTN_TOUCH && pos_buf.value == 0){     //松手
            tc_state.end_pos.x = pos_x;
            tc_state.end_pos.y = pos_y;
            tc_state.x_ready = false;
            tc_state.y_ready = false;
        }
        // if(pos_buf.type == EV_SYN && pos_x && pos_y){                         //每分割一次数据包就更新一次坐标差
        //     // printf("当前坐标:(%d,%d)\n", pos_x, pos_y);
        //     if(pos_x > pre_x){
        //         tc_state.r_slide = true;
        //         tc_state.l_slide = false;
        //         x_diff = pos_x - pre_x;
        //     }       
        //     else if(pos_x < pre_x){
        //         tc_state.r_slide = false;
        //         tc_state.l_slide = true;
        //     }  
        //     pre_x = pos_x;
        // }
    }
}



void slide_right2left(){
    printf("进来了了右滑图片函数\n");
    int (* dst)[WIDTH] = (int (*)[WIDTH])map;
    int (* src)[3*WIDTH] = (int (*)[3*WIDTH])reserve_mem;
    for(int j = 0; j < HEIGHT; j++){
        //左边出图
        for(int i = 0; i < pos_x; i++){
            // memcpy( (dst + ((j) * WIDTH + i)), (reserve_mem + (j*2400 + (WIDTH-pos_x+i))), 4);
            dst[j][i] = src[j][WIDTH-pos_x+i];
            // dst[j][i] = src[j][WIDTH-pos_x+i];
        }
        //右边消图
        for(int i = 0; i < WIDTH-pos_x; i++){
            // memcpy( (map + (j*WIDTH + pos_x+i)), (reserve_mem + j*2400 + 800+i), 4);
            dst[j][pos_x + i] = src[j][800+i];
            // dst[j][pos_x + i] = src[j][800+i];
        }
#if DEBUG
        for(int i = 0; i < WIDTH; i++){
            memcpy( (dst + ((j) * WIDTH + i)), (reserve_mem + (j*2400 + (i))), 4);
        }
#endif
    }
}

void slide_left2right(){
    printf("进来了了左滑图片函数\n");
    int (* dst)[WIDTH] = (int (*)[WIDTH])map;
    int (* src)[3*WIDTH] = (int (*)[3*WIDTH])reserve_mem; 

    for(int j = 0; j < HEIGHT; j++){
        //左边消图
        for(int i = 0; i < pos_x; i++){
            // memcpy( (dst + ((j) * WIDTH + i)), (reserve_mem + (j*2400 + (WIDTH-pos_x+i))), 4);
            dst[j][i] = src[j][WIDTH+800-pos_x+i];
        }
        //右边出图
        for(int i = 0; i < WIDTH-pos_x; i++){
            // memcpy( (map + (j*WIDTH + pos_x+i)), (reserve_mem + j*2400 + 800+i), 4);
            dst[j][pos_x + i] = src[j][1600+i];
        }

#if DEBUG   
    //根据差值多少来改变映射区的像素列
    for(int i = 0; i < pos_x; i++){
        for(int j = 0; j < HEIGHT; j++){
            // memcpy();        
            memcpy(&((unsigned long(*)[WIDTH])map)[j][WIDTH-i-1], &((unsigned long(*)[WIDTH])map)[j+480][0], 4*(i+1));
        }
    }
#endif
    }
}