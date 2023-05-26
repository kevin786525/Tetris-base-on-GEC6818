#include "lcd.h"


//============================全局变量定义============================//

int fd_lcd = -1;
//获取屏幕的固定属性
struct fb_fix_screeninfo fixinfo;
struct fb_var_screeninfo varinfo; // 可变属性

//============================全局变量定义============================//


//功  能:初始化lcd屏幕文件
void LCD_Init(void){

    fd_lcd = open("/dev/fb0", O_RDWR);
    if(fd_lcd < 0){
        perror("打开lcd文件失败!");
        exit(0);
    }
}

//功  能:显示屏幕设备的信息
void show_lcd_info()
{
    // 获取LCD设备硬件fix属性
    get_fixinfo();
    printf("\n获取LCD设备固定属性信息成功：\n");
    printf("[ID]: %s\n", fixinfo.id);
    printf("[FB类型]: ");
    switch(fixinfo.type)
    {
        case FB_TYPE_PACKED_PIXELS:      printf("组合像素\n");break;
        case FB_TYPE_PLANES:             printf("非交错图层\n");break;
        case FB_TYPE_INTERLEAVED_PLANES: printf("交错图层\n");break;
        case FB_TYPE_TEXT:               printf("文本或属性\n");break;
        case FB_TYPE_VGA_PLANES:         printf("EGA/VGA图层\n");break;
    }
    printf("[FB视觉]: ");
    switch(fixinfo.visual)
    {
        case FB_VISUAL_MONO01:             printf("灰度. 1=黑;0=白\n");break;
        case FB_VISUAL_MONO10:             printf("灰度. 0=黑;1=白\n");break;
        case FB_VISUAL_TRUECOLOR:          printf("真彩色\n");break;
        case FB_VISUAL_PSEUDOCOLOR:        printf("伪彩色\n");break;
        case FB_VISUAL_DIRECTCOLOR:        printf("直接彩色\n");break;
        case FB_VISUAL_STATIC_PSEUDOCOLOR: printf("只读伪彩色\n");break;
    }
    printf("[行宽]: %d 字节\n", fixinfo.line_length);

    // 获取LCD设备硬件var属性
    get_varinfo();
    printf("\n获取LCD设备可变属性信息成功：\n");
    printf("[可见区分辨率]: %d×%d\n", varinfo.xres, varinfo.yres);
    printf("[虚拟区分辨率]: %d×%d\n", varinfo.xres_virtual, varinfo.yres_virtual);
    printf("[从虚拟区到可见区偏移量]: (%d,%d)\n", varinfo.xoffset, varinfo.yoffset);
    printf("[色深]: %d bits\n", varinfo.bits_per_pixel);
    printf("[像素内颜色结构]:\n");
    printf("  [透明度] 偏移量:%d, 长度:%d bits\n", varinfo.transp.offset, varinfo.transp.length);
    printf("  [红] 偏移量:%d, 长度:%d bits\n", varinfo.red.offset, varinfo.red.length);
    printf("  [绿] 偏移量:%d, 长度:%d bits\n", varinfo.green.offset, varinfo.green.length);
    printf("  [蓝] 偏移量:%d, 长度:%d bits\n", varinfo.blue.offset, varinfo.blue.length);
    printf("\n");
}


//功  能:获取LCD设备的固定属性
void get_fixinfo(){
    if(ioctl(fd_lcd, FBIOGET_FSCREENINFO, &fixinfo) != 0)
    {
        perror("获取LCD设备固定属性信息失败");
        return;
    }
}

//功  能:获取LCD设备的可变属性
void get_varinfo(){
    if(ioctl(fd_lcd, FBIOGET_VSCREENINFO, &varinfo) != 0)
    {
        perror("获取LCD设备可变属性信息失败");
        return;
    }
}
