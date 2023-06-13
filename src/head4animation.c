#include "Main_Function.h"
#include "list.h"
#include "commonheader.h"
#include "Kernel_List.h"
#include "my_types.h"
#include "bmp.h"
#include "font.h"
#include "head4animation.h"
#include <math.h>


//从上而下显示图片
void fall_down_in(int lcd, unsigned long (*image)[WIDTH]){

    //申请一块适当大小的frame-buffer 映射到LCD
    unsigned long (*FB)[WIDTH] = mmap(NULL, SCREEN_SIZE*4,      //实际上就是两块屏幕的内存大小 
                                            PROT_READ | PROT_WRITE,
                                            MAP_SHARED, lcd, 0);
    
    for(int i = 0;  i < HEIGHT; i++){
        memcpy(&FB[0][0], &image[HEIGHT-i-1][0], WIDTH*4*(i+1));
        usleep(1000);
    }
}

//从上而下消除图片
void fall_down_out(int lcd, unsigned long (*image)[WIDTH]){

    //申请一块适当大小的frame-buffer 映射到LCD
    unsigned long (*FB)[WIDTH] = mmap(NULL, SCREEN_SIZE*4,      //实际上就是两块屏幕的内存大小 
                                            PROT_READ | PROT_WRITE,
                                            MAP_SHARED, lcd, 0);
    
    for(int i = 0;  i <= HEIGHT; i++){
        memset(&FB[0][0], 0, WIDTH*4*i);
        memcpy(&FB[i][0], &image[0][0], WIDTH*4*(HEIGHT-i));
        usleep(1000);
    }
}


//图片从上而下掉落，穿过显示屏
void fall_down(int lcd, unsigned long (*image)[WIDTH]){

    fall_down_in(lcd, image);
    fall_down_out(lcd, image);
}


void get_image(P_node node, unsigned long (*buf)[WIDTH]){

    // printf("当前需要打开的图片是---->%s\n",node->Data.name);
    FILE *fp = fopen(node->Data.name, "r");
    struct bitmap_info bmp_info;
    bmp_info = read_bmp_head(fp);
    //如果图片横向分辨率不为能被4整除的整数，则计算需要补齐的字节数(当前图片)
    int polish = 0;
    if( (bmp_info.width * 3) % 4 != 0 ){
        polish = 4 - ((bmp_info.width * 3) % 4);        //int pad = ((4-( width * bpp/8 ) % 4)) % 4;
    }
    
    //开始读取当前图片像素点数据
    int rgb_size = (bmp_info.width * 3 + polish) * bmp_info.height;
    char rgb_buf[rgb_size];
    if( fread(rgb_buf, rgb_size, 1, fp) != 1 ){
        perror("读取图片像素点数据失败!");
        // continue; 
    }

    //当前图片合成像素点
    int lcd_buf [bmp_info.height][bmp_info.width];
    for(int y = 0; y < bmp_info.height; y++){
        for(int x = 0; x < bmp_info.width; x++){
            lcd_buf[bmp_info.height - y -1][x] =        rgb_buf[ (x+y*bmp_info.width)*3 + 0 + y *polish ] << 0  |
                                                        rgb_buf[ (x+y*bmp_info.width)*3 + 1 + y *polish ] << 8  |
                                                        rgb_buf[ (x+y*bmp_info.width)*3 + 2 + y *polish ] << 16 |
                                                        0x00 << 24;
        }
    }
    memcpy(buf, lcd_buf, bmp_info.height * bmp_info.width * 4);
}

void float_up_in(int lcd, unsigned long (*image)[WIDTH]){

    unsigned long (*FB)[WIDTH] = mmap(NULL, SCREEN_SIZE*4,      //实际上就是两块屏幕的内存大小 
                                            PROT_READ | PROT_WRITE,
                                            MAP_SHARED, lcd, 0);
    
    int i = 0;
    while(1){
        memcpy(&FB[HEIGHT-i-1][0], &image[0][0], WIDTH*4*(i+1));
        if(i >= HEIGHT - 1) break;
        usleep(1000);
        i++;
    }
}

void float_up_out(int lcd, unsigned long (*image)[WIDTH]){

    unsigned long (*FB)[WIDTH] = mmap(NULL, SCREEN_SIZE*4,      //实际上就是两块屏幕的内存大小 
                                            PROT_READ | PROT_WRITE,
                                            MAP_SHARED, lcd, 0);
    
    for(int i = 0; i <= HEIGHT; i++){
        memset(&FB[HEIGHT-i][0], 0, WIDTH*4*i);
        memcpy(&FB[0][0], &image[i][0], WIDTH*4*(HEIGHT-i));
        usleep(1000);
    }
}

void float_up(int lcd, unsigned long (*image)[WIDTH]){
    float_up_in(lcd, image);
    float_up_out(lcd, image);
}


void left2right_in(int lcd, unsigned long (*image)[WIDTH]){

    unsigned long (*FB)[WIDTH] = mmap(NULL, SCREEN_SIZE*4,      //实际上就是两块屏幕的内存大小
                                            PROT_READ | PROT_WRITE,
                                            MAP_SHARED, lcd, 0);

    for(int i = 0; i < WIDTH; i++){
        for(int j = 0; j < HEIGHT; j++){
            memcpy(&FB[j][0], &image[j][WIDTH-1-i], 4*(i+1));
        }
    }
}


void left2right_out(int lcd, unsigned long (*image)[WIDTH]){

    unsigned long (*FB)[WIDTH] = mmap(NULL, SCREEN_SIZE*4,      //实际上就是两块屏幕的内存大小 
                                            PROT_READ | PROT_WRITE,
                                            MAP_SHARED, lcd, 0);
    for(int i = 0; i < WIDTH; i++){
        for(int j = 0; j < HEIGHT; j++){
            memset(&FB[j][0], 0, 4*(i+1));
            memcpy(&FB[j][i+1], &image[j][0], (WIDTH-1-i)*4);
        }
    }
}


void left2right(int lcd, unsigned long (*image)[WIDTH]){

    left2right_in(lcd, image);
    left2right_out(lcd, image);
}


void * routine(void * p){
    struct argument * arg = (struct argument *)p;

    for(int i = 0; i < HEIGHT/BLIND; i++){
        if(arg->flag == IN){
            memcpy(&(arg->FB)[arg->offset+i][0], 
                    &(arg->image)[arg->offset+i][0],
                    WIDTH*4);
        }
        if(arg->flag == OUT){
            memset(&(arg->FB)[arg->offset+i][0],
                    0,
                    WIDTH*4);
        }
        usleep(1000);
    }
    pthread_exit(NULL);
}


void __write_lcd(int lcd, unsigned long (*image)[WIDTH], int flag){
    unsigned long (*p)[WIDTH] = mmap(NULL, SCREEN_SIZE * 4,
                                        PROT_READ | PROT_WRITE,
                                        MAP_SHARED, lcd, 0);
    pthread_t tid[BLIND];
    for(int i = 0; i < BLIND; i++){
        struct argument * arg = malloc(sizeof(struct argument));
        arg->FB     = p;                //buf指针
        arg->image  = image;            //图片数据缓冲区指针
        arg->offset = i*(HEIGHT/BLIND); //第i条线程负责区域的偏移量
        arg->flag   = flag;             //IN为显示图片，OUT为消除图片

        //创建一条线程并将参数arg传入，
        pthread_create(&tid[i], NULL, routine, (void *)arg);
    }
    for(int i = 0; i < BLIND; i++)
        pthread_join(tid[i], NULL);
}

void blind_window_in(int lcd, unsigned long(*image)[WIDTH]){
    __write_lcd(lcd, image, IN);
}
void blind_window_out(int lcd, unsigned long(*image)[WIDTH]){
    __write_lcd(lcd, image, OUT);
}
void blind_window(int lcd, unsigned long(*image)[WIDTH]){
    blind_window_in(lcd, image);
    blind_window_out(lcd, image);
}

