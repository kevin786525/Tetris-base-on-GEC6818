#include "font.h"
#include "head4animation.h"
extern int * map;

//在指定位置显示指定大小的内容
Bitmap_Font * font_pos_size_data(int bm_width, int bm_height, int bm_x, int bm_y, int x, int y, char * str){
    //打开字体
    font * f = fontLoad(FONT_TTF);
    //创建面板
    bitmap * bm = createBitmapWithInit(bm_width, bm_height, 4, getColor(0, 255, 255, 255));
    //设置字体大小
    fontSetSize(f, 32);
    //将字体嵌入到画板中
    fontPrint(f, bm, x, y, str, getColor(0, 146, 211, 13), 512);
    //将字体投影到lcd上
    show_font_to_lcd(map, bm_x, bm_y, bm);
    Bitmap_Font * bf = calloc(1, sizeof(Bitmap_Font));
    bf->b = bm;
    bf->f = f;
    // sleep(3);
    return bf;
}