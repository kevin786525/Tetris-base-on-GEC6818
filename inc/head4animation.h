#ifndef _HEAD4ANIMATION_H_
#define _HEAD4ANIMATION_H_

#include "commonheader.h"
#include "my_types.h"


#define SCREEN_SIZE 800 * 480
#define WIDTH   800
#define HEIGHT  480
#define BLIND   5    //实现百叶窗效果时的线程数
#define IN      1
#define OUT     0

struct argument{
    unsigned long (*FB)[WIDTH];
    unsigned long (*image)[WIDTH];
    int offset;
    int flag;
};

void fall_down_in(int lcd, unsigned long (*image)[WIDTH]);
void fall_down_out(int lcd, unsigned long (*image)[WIDTH]);
void fall_down(int lcd, unsigned long (*image)[WIDTH]);

void float_up_in(int lcd, unsigned long (*image)[WIDTH]);
void float_up_out(int lcd, unsigned long (*image)[WIDTH]);
void float_up(int lcd, unsigned long (*image)[WIDTH]);

void left2right_in(int lcd, unsigned long (*image)[WIDTH]);
void left2right_out(int lcd, unsigned long (*image)[WIDTH]);
void left2right(int lcd, unsigned long (*image)[WIDTH]);

void blind_window_in(int lcd, unsigned long(*image)[WIDTH]);
void blind_window_out(int lcd, unsigned long(*image)[WIDTH]);
void blind_window(int lcd, unsigned long(*image)[WIDTH]);

void get_image(P_node node, unsigned long (*buf)[WIDTH]);

#endif