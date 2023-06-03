#include "weather.h"

#define WEATHER_FILE "./tmp.txt"


//============================全局变量定义============================//
//定义两个普通全局变量，让工程内可见
extern int fd_lcd;
extern int fd_tc;

extern unsigned long pos_x;
extern unsigned long pos_y;

extern unsigned int out_x;
extern unsigned int out_y; // 松手坐标值
extern unsigned int pre_x;

extern unsigned int x_diff;

//伪状态机
extern bool is_playing_video;
extern bool change_bmp;

extern int * map;

//获取屏幕的固定属性
extern struct fb_fix_screeninfo fixinfo;
extern struct fb_var_screeninfo varinfo; // 可变属性
extern struct input_event pos_buf;
extern struct touch_state tc_state;


//============================全局变量定义============================//

//功  能:读取天气预报文件
void read_weather_forecast(char  argv[][128]){
    FILE * fp = fopen(WEATHER_FILE, "r");
    char tmp[128];
    
    int i = 0;
    while(fgets(tmp, 128, fp) != NULL){
        //处理接收到的字符串的最后一个换行符，换成字符串结束符
        tmp[strlen(tmp)-1] = '\0';
        memcpy(argv[i++], tmp, 128);
    }
    fclose(fp);
}

//功  能:滚动刷新天气
void *flash_weather(void * arg){
    //阻塞1s等待刷新的开机画面
    sleep(1);
    char buf[6][128];
    memset(buf, 0, 6*128);
    read_weather_forecast(buf);
    for(int i = 0; i < 6; ){
        printf("\t---->%s\n", buf[i++]);
    }
    //打开字体
    font * f = fontLoad("/usr/share/fonts/simfang.ttf");
    //创建天气画板
    bitmap * bm_right= createBitmapWithInit(450,30,4,getColor(0,255,255,255));
    //设置字体大小
    fontSetSize(f, 24);
    //存储右边背景图的数据
    char back_right[450*30*4];
    memset(back_right, 0, 300*30*4);
    //获取首张画面的背景数据
    font_background( bm_right, 301, 0);
    memcpy(back_right, bm_right->map, 450*30*4);
    int start_pos = 450;
    while(1){
        //检测到播放视频时,阻塞显示
        if(is_playing_video){
            continue;
        }
        //检测到更换图片时，保存图片的背景数据
        if(change_bmp){
            change_bmp = false;
            //获取背景数据
            font_background( bm_right, 301, 0 );
            //将背景数据备份到备份内存内
            memcpy(back_right, bm_right->map, 450*30*4);
        }
        //将字体嵌入到画板中
        fontPrint(f, bm_right, start_pos, 0, buf[0], getColor(0, 34, 231, 255), 800);
        //将字体投映到lcd上
        show_font_to_lcd(map, 300, 0, bm_right);
        start_pos--;
        if(start_pos < -(3*24))
            start_pos = 450;
        usleep(100000);
        memcpy(bm_right->map, back_right, 450*30*4);
    }
}

//功  能:在状态栏刷新时间,以及滚动显示天气信息
void * flash_time(void * arg){
    //先等待刷新图片
    sleep(1);
   
    char buf[6][128];
    memset(buf, 0, 6*128);
    read_weather_forecast(buf);
    for(int i = 0; i < 6; ){
        printf("\t---->%s\n", buf[i++]);
    }
    //打开字体
    font * f = fontLoad("/usr/share/fonts/simfang.ttf");

    //创建时间画板
    bitmap * bm_left = createBitmapWithInit(300,30,4,getColor(0,255,255,255));
    //设置字体大小
    fontSetSize(f, 24);
    //存储左边背景图的数据
    char back_left[300*30*4];
    memset(back_left, 0, 300*30*4);
    font_background( bm_left, 0, 0 );
    memcpy(back_left, bm_left->map, 300*30*4);

    //创建天气画板
    bitmap * bm_right= createBitmapWithInit(450,30,4,getColor(0,255,255,255));
    //存储右边背景图的数据
    char back_right[450*30*4];
    memset(back_right, 0, 300*30*4);
    //获取首张画面的背景数据
    font_background( bm_right, 301, 0);
    memcpy(back_right, bm_right->map, 450*30*4);
    int start_pos = 450;
    int i = 0;
    while(1){
        //当检测到播放视频时，将继续循环，相当于阻塞状态
        if(is_playing_video){
            continue;
        }
        time_t timep; 
        time (&timep);
        char * p_time = ctime(&timep);
        //检测是否切换图片,如果是切换图片时，则保存图片的背景数据
        if(change_bmp){
            change_bmp = false;
            //设定延时，等待图片刷新完毕
            sleep(1);
            //获取背景数据
            font_background( bm_left, 0, 0 );
            //将背景数据备份到备份内存内
            memcpy(back_left, bm_left->map, 300*30*4);
            //获取背景数据
            font_background( bm_right, 301, 0 );
            //将背景数据备份到备份内存内
            memcpy(back_right, bm_right->map, 450*30*4);
            
        }

        //将时间信息写入到画板中
        fontPrint(f, bm_left, 0, 0, p_time, getColor(0, 34, 231, 255), 800);
        
        //将时间框输出到LCD屏幕上
        show_font_to_lcd(map, 0, 0, bm_left);
        // sleep(1);
        //睡1s之后利用背景图的数据覆盖文字实现清屏
        memcpy(bm_left->map, back_left, 300*30*4);
        //将天气嵌入到画板中
        fontPrint(f, bm_right, start_pos, 5, buf[i], getColor(0, 34, 231, 255), 800);
        //将天气投映到lcd上
        show_font_to_lcd(map, 300, 0, bm_right);
        start_pos--;
        if(start_pos < -(24*24)){
            start_pos = 450;
            i++;
        }
        if(i > 5)
            i = 0;
            
        usleep(100000);
        // sleep(1);
        memcpy(bm_right->map, back_right, 450*30*4);
    }
}

//改变字体的背景
void font_background(bitmap * bm, int x, int y){
    //提取背景的像素数据，填充到bm上
    unsigned int * p = ((unsigned int *)map + x + y*800);
    for(int i =  0; i < bm->height; i++){
        memcpy(bm->map+i*bm->width*4, p, bm->width*4);
        p += 800;
    }
}