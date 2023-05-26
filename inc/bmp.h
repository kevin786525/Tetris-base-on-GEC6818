/////////////////////////////////////////////
//
//    备注：此头文件包含了一些操作及存储图片头部数据
//    的结构体
//
//
/////////////////////////////////////////////
#ifndef __BMP_H_
#define __BMP_H_

#include "commonheader.h"
#include "my_types.h"
#include "Kernel_List.h"
#include "Main_Function.h"
#include "head4animation.h"
#include "touch.h"

struct bitmap_header
{
	int16_t type;
	int32_t size; // 图像文件大小
	int16_t reserved1;
	int16_t reserved2;
	int32_t offbits; // bmp图像数据偏移量
}__attribute__((packed));

struct bitmap_info
{
	int32_t size; // 本结构大小	
	int32_t width;
	int32_t height;
	int16_t planes; // 总为零

	int16_t bit_count; // 色深
	int32_t compression;
	int32_t size_img; // bmp数据大小，必须是4的整数倍
	int32_t X_pel;
	int32_t Y_pel;
	int32_t clrused;
	int32_t clrImportant;
}__attribute__((packed));

/*struct rgb_quad        //映射到开发板子上面屏幕的单个字节的像素结构体
{
	int8_t blue;
	int8_t green;
	int8_t red;
	int8_t reserved;
}__attribute__((packed));*/


//功  能: 申请一块三块LCD屏幕大小的内存块以存储三张图片数据
void mem_Init(void);



//函数名:Display_photos()
//功  能:显示图片
//参  数:@name目标图片的名字
//返回值:无
void Display_photos(P_node my_list, int * map);

//函数名:read_bmp_head()
//功  能:读取bmp图片的54字节头部信息
struct bitmap_info read_bmp_head(FILE * fp);

//函数名:display_node()
//功  能:展示当前节点的图片
void display_node(P_node now);

//函数名:show_album()
//功  能:浏览相册
void show_album(P_node my_list);

//函数名:display_next_photo()
//功  能:显示下一张图片
P_node display_next_photo(P_node pos, P_node my_list);
//功  能:显示上一张图片
P_node display_prev_photo(P_node pos, P_node my_list);

//功  能: 将前后节点的图片数据写进虚拟显存的b、c区域
void prepare_node(P_node cur, unsigned int width, unsigned int height);

//功  能: 刷新相册里面的缩略图栏
void flush_state_line(P_node cur);

//功  能: 定位显示图片，及分辨率
int * lcd_pos_size_pixel(P_node node, const unsigned int x, const unsigned int y, int width, int height);

//功  能: 点击放大图片
P_node click2show(P_node cur);
#endif