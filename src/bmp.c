#include "bmp.h"

#define DEBUG 0

#define VERSION 0


//============================全局变量定义============================//

int * reserve_mem = NULL;

P_node bmpList;

//伪状态机
bool change_bmp = false;
bool is_playing_video = false;


extern int fd_lcd;
extern int * map;

extern unsigned long pos_x;
extern unsigned long pos_y;


//获取屏幕的固定属性
extern struct fb_fix_screeninfo fixinfo;
extern struct fb_var_screeninfo varinfo; // 可变属性
extern struct touch_state tc_state;
extern struct input_event pos_buf;


//============================全局变量定义============================//


//功  能: 申请一块三块LCD屏幕大小的内存块以存储三张图片数据
void mem_Init(void){                    //宽            高              色深
	// reserve_mem = calloc(1, 3*(varinfo.xres * varinfo.yres * varinfo.bits_per_pixel/8));
	reserve_mem = calloc(1, 3*(800 * 480 * 4));
    if(reserve_mem == NULL){
        printf("申请储备内存失败\n");
        return ;
    }
}


//功  能:显示图片
void Display_photos(P_node my_list, int * map){

    if(IS_NULL(my_list))
        printf("当前链表为空!\n");

    P_node cur = NULL;
    P_node nex = NULL;

    //设定可见区
    ioctl(fd_lcd, FBIOGET_VSCREENINFO, &varinfo); // 获取可变属性
    varinfo.xoffset = 0;
    varinfo.yoffset = 0;


    list_for_each_entry_safe(cur, nex, &my_list->ptr, ptr){
    
        //首先打开图片  
        FILE *fp_cur = fopen(cur->Data.name, "r"); //获取当前节点信息
        //创建bmp头部信息结构体
        struct bitmap_header bmp_head_cur;
        memset(&bmp_head_cur, 0, sizeof(bmp_head_cur));

        if( fread(&bmp_head_cur, sizeof(bmp_head_cur), 1, fp_cur) != 1 ){
            perror("读取bmp_cur图片头部信息失败!");
            return ;
        }

        //读取bmp的信息头
        struct bitmap_info bmp_info_cur;
        memset(&bmp_info_cur, 0, sizeof(bmp_info_cur));

        if( fread(&bmp_info_cur, sizeof(bmp_info_cur), 1, fp_cur) != 1 ){
            perror("读取bmp_cur图片信息头失败!");
            return ;
        }
        
        //打印当前图片部分信息
        printf("当前图片名字为:%s\n", cur->Data.name);
        printf("=====分割线=================分割线=====\n");
        printf("\n");

        //如果图片横向分辨率不为能被4整除的整数，则计算需要补齐的字节数(当前图片)
        int polish_cur = 0;
        if( (bmp_info_cur.width * 3) % 4 != 0 ){
            polish_cur = 4 - ((bmp_info_cur.width * 3) % 4);
        }
        
        // printf("---->当前行号:%d\n",__LINE__);
        //开始读取当前图片像素点数据
        int rgb_size_cur = (bmp_info_cur.width * 3 + polish_cur) * bmp_info_cur.height;
        char rgb_buf_cur[rgb_size_cur];
        if( fread(rgb_buf_cur, rgb_size_cur, 1, fp_cur) != 1 ){
            perror("读取图片像素点数据失败!");
            continue;
        }

        //当前图片合成像素点
        int lcd_buf_cur [bmp_info_cur.height][bmp_info_cur.width];
        for(int y = 0; y < bmp_info_cur.height; y++){
            for(int x = 0; x < bmp_info_cur.width; x++){
                lcd_buf_cur[bmp_info_cur.height - y -1][x] =    rgb_buf_cur[ (x+y*bmp_info_cur.width)*3 + 0 + y *polish_cur ] << 0  |
                                                                rgb_buf_cur[ (x+y*bmp_info_cur.width)*3 + 1 + y *polish_cur ] << 8  |
                                                                rgb_buf_cur[ (x+y*bmp_info_cur.width)*3 + 2 + y *polish_cur ] << 16 |
                                                                0x00 << 24;
            }
        }
        struct bitmap_header bmp_head_nex;
        struct bitmap_info bmp_info_nex;
        int lcd_buf_nex [bmp_info_nex.height][bmp_info_nex.width];

        //如果下一张已经到头节点，则指向头节点的下一个节点，实现循环
        if(&nex->ptr == &my_list->ptr)
            nex = list_entry(my_list->ptr.next, typeof(*nex), ptr);

        FILE *fp_nex = fopen(nex->Data.name, "r"); //获取当前节点的下一个节点的信息
        memset(&bmp_head_nex, 0, sizeof(bmp_head_nex));
        if( fread(&bmp_head_nex, sizeof(bmp_head_nex), 1, fp_nex) != 1 ){
            perror("读取bmp_nex图片头部信息失败!");
            return ;
        }   
        memset(&bmp_info_nex, 0, sizeof(bmp_info_nex));
        if( fread(&bmp_info_nex, sizeof(bmp_info_nex), 1, fp_nex) != 1 ){
            perror("读取bmp_nex图片信息头失败!");
            return ;
        }
        //打印下一张图片部分信息
        printf("下一张图片名字为:%s\n", nex->Data.name);
        printf("=====分割线=================分割线=====\n");
        printf("\n");

        //如果图片横向分辨率不为能被4整除的整数，则计算需要补齐的字节数(下一张图片)
        int polish_nex = 0;
        if( (bmp_info_nex.width * 3) % 4 != 0 ){
            polish_nex = 4 - ((bmp_info_nex.width * 3) % 4);
        }
        //开始读取下一张图片像素点数据
        int rgb_size_nex = (bmp_info_nex.width * 3 + polish_nex) * bmp_info_nex.height;
        char rgb_buf_nex[rgb_size_nex];
        if( fread(rgb_buf_nex, rgb_size_nex, 1, fp_nex) != 1 ){
            perror("读取图片像素点数据失败!");
            continue;
        }
        //下一张图片合成像素点
        for(int y = 0; y < bmp_info_nex.height; y++)
            for(int x = 0; x < bmp_info_nex.width; x++)
                lcd_buf_nex[bmp_info_nex.height - y -1][x] =    rgb_buf_nex[ (x+y*bmp_info_nex.width)*3 + 0 + y *polish_nex ] << 0  |
                                                                rgb_buf_nex[ (x+y*bmp_info_nex.width)*3 + 1 + y *polish_nex ] << 8  |
                                                                rgb_buf_nex[ (x+y*bmp_info_nex.width)*3 + 2 + y *polish_nex ] << 16 |
                                                                0x00 << 24;
        ioctl(fd_lcd, FBIOPAN_DISPLAY, &varinfo);
        //首先将当前图片显示在A区
        for(int y = 0; y < bmp_info_cur.height; y++)
            for(int x = 0; x < bmp_info_cur.width; x++)
                if(x > 0 && x < 799 && y > 0 && y < 479)
                    *(map + x + y*800) = lcd_buf_cur[y][x];
        sleep(1);

        //将下一张图片画在b区
        for(int y = 0; y < bmp_info_nex.height; y++)
            for(int x = 0; x < bmp_info_nex.width; x++)
                if(x > 0 && x < 799 && y > 0 && y < 479)
                    *(map + x + (y+480)*800) = lcd_buf_nex[y][x];
        sleep(1);

        while(1){
            if(varinfo.yoffset == 480)  break;
            ioctl(fd_lcd, FBIOPAN_DISPLAY, &varinfo);
            varinfo.yoffset+=1;
            printf("当前y的偏移量为:%d\n", varinfo.yoffset);
            // usleep(10);             //10μs
            // usleep(100);            //100μs
            // usleep(1000);           //1ms
            usleep(10000);          //10ms
            // usleep(100000);         //0.1s
            // sleep(1);               //1s
        }
    }       

}

//功  能:读取bmp图片的54字节头部信息
struct bitmap_info read_bmp_head(FILE * fp){

    if(fp == NULL){
        printf("图片名字参数异常\n");
        // return 0;
    }
    //创建bmp头部信息结构体
    struct bitmap_header bmp_head;
    memset(&bmp_head, 0, sizeof(bmp_head));

    if( fread(&bmp_head, sizeof(bmp_head), 1, fp) != 1 ){
        perror("读取bmp_cur图片头部信息失败!");
        // return 0;
    }
    //读取bmp的信息头
    struct bitmap_info bmp_info;
    memset(&bmp_info, 0, sizeof(bmp_info));

    if( fread(&bmp_info, sizeof(bmp_info), 1, fp) != 1 ){
        perror("读取bmp_cur图片信息头失败!");
        // return 0;
    }
    return bmp_info;
}

//功  能:展示当前节点的图片
void display_node(P_node now){

    //每当进入函数时，就证明需要切换图片
    change_bmp = true;
    
    //打开图片  
    printf("当前需要打开的图片是---->%s\n",now->Data.name);
    FILE *fp = fopen(now->Data.name, "r");
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
    //将图片显示在a区
    for(int y = 0; y < bmp_info.height; y++)
        for(int x = 0; x < bmp_info.width; x++)
            if(x > 0 && x < 799 && y > 0 && y < 479)
                *(map + x + y*800) = lcd_buf[y][x];
    // ioctl(fd_lcd, FBIOPAN_DISPLAY, &varinfo);

    
#if DEBUG
    sleep(1);
    varinfo.yoffset = 480;
    varinfo.xoffset = 480;
    ioctl(fd_lcd, FBIOPAN_DISPLAY, &varinfo);
#endif
}


//功  能:浏览相册
void show_album(P_node my_list){

    //将要显示的图片从主链表中分离到单独的链表中
    bmpList = List_Init();
    bmpList = handle_list(bmpList, my_list);

    //显示相册的背景图片
    P_node album_back = search_2_list(my_list, "album_back");
    display_node(album_back);


    P_node cur   = get_next_node(bmpList, bmpList);
    flush_state_line(cur);
    prepare_node(cur, 400, 240);

    while(1){
        // display_node(album_back);
        // flush_state_line(cur);
        if(pos_x > 0 && pos_x < 70 && pos_y > 410 && pos_y < 800){
            printf("退出相册\n");
            pos_x = 0;
            pos_y = 0;
            break;
        }
        else if((pos_x > 725 && pos_x < 775 && pos_y > 50 && pos_y < 95) || (pos_x > 550  && pos_x < 650 && pos_y > 50 && pos_y < 110)){
            printf("点击了下一张图片\n");
            cur = display_next_photo(cur, bmpList);
            pos_x = 0;
            pos_y = 0;
        }            //显示下一张
        else if((pos_x > 19  && pos_x < 65 && pos_y > 45 && pos_y < 90) || (pos_x > 150  && pos_x < 250 && pos_y > 50 && pos_y < 110)){
            printf("点击了上一张图片\n");
            cur = display_prev_photo(cur, bmpList);
            pos_x = 0;
            pos_y = 0;
        }
        else if((pos_x > 200  && pos_x < 600 && pos_y > 240 && pos_y < 480)){
            printf("放大当前图片\n");
            cur = click2show(cur);
            display_node(album_back);
            flush_state_line(cur);
            pos_x = 0;
            pos_y = 0;
        }
    }
}

#if VERSION
//功  能: 将前后节点的图片数据写进虚拟显存的b、c区域
void prepare_node(P_node cur, unsigned int width, unsigned int height){

    //每当进入函数时，就证明需要切换图片
    change_bmp = true;

    P_node nex = get_next_node(cur, bmpList);
    P_node pre = get_prev_node(cur, bmpList);

    //打开三个节点的图片
    FILE * fp_cur = fopen(cur->Data.name, "r");
    FILE * fp_nex = fopen(nex->Data.name, "r");
    FILE * fp_pre = fopen(pre->Data.name, "r");

    struct bitmap_info cur_info;
    struct bitmap_info nex_info;
    struct bitmap_info pre_info;

    cur_info = read_bmp_head(fp_cur);
    nex_info = read_bmp_head(fp_nex);
    pre_info = read_bmp_head(fp_pre);

    int pad_cur = 0;
    int pad_nex = 0;
    int pad_pre = 0;

    if( (cur_info.width * 3) % 4 != 0 ) pad_cur = 4 - ((cur_info.width * 3) % 4);
    if( (nex_info.width * 3) % 4 != 0 ) pad_nex = 4 - ((nex_info.width * 3) % 4);
    if( (pre_info.width * 3) % 4 != 0 ) pad_pre = 4 - ((pre_info.width * 3) % 4);

    //开始读取当前图片像素点数据
    int rgb_size_cur = (cur_info.width * 3 + pad_cur) * cur_info.height;
    char cur_rgb_buf[rgb_size_cur];
    if( fread(cur_rgb_buf, rgb_size_cur, 1, fp_cur) != 1 ){
        perror("读取cur节点图片像素点数据失败!");
        // continue; 
    }
    int rgb_size_nex = (nex_info.width * 3 + pad_nex) * nex_info.height;
    char nex_rgb_buf[rgb_size_nex];
    if( fread(nex_rgb_buf, rgb_size_nex, 1, fp_nex) != 1 ){
        perror("读取nex节点图片像素点数据失败!");
        // continue; 
    }
    int rgb_size_pre = (pre_info.width * 3 + pad_pre) * pre_info.height;
    char pre_rgb_buf[rgb_size_pre];
    if( fread(pre_rgb_buf, rgb_size_pre, 1, fp_pre) != 1 ){
        perror("读取pre节点图片像素点数据失败!");
        // continue; 
    }

    printf("当前在%s函数内，开始合成a区的图片数据\n", __FUNCTION__);

    //cur图片合成像素点
    int cur_lcd_buf [height][width];
    unsigned int x_diff = cur_info.width / width;   //计算宽度的每次偏移量
    unsigned int y_diff = cur_info.height / height; //计算高度的每次偏移量

    for(int j = 0, J = 0; j < height; j++, J+=y_diff){
        for(int i = 0, I = 0; i < width; i++, I+=x_diff){
            cur_lcd_buf[height - j -1][i] = cur_rgb_buf[ (J*cur_info.width+I)*3 + 0 + J*pad_cur ] << 0  |   //B
                                            cur_rgb_buf[ (J*cur_info.width+I)*3 + 1 + J*pad_cur ] << 8  |   //G
                                            cur_rgb_buf[ (J*cur_info.width+I)*3 + 2 + J*pad_cur ] << 16 |   //R
                                            0x00 << 24;                                                //A
        }
    }
    //将cur图片显示在a区
    for(int j = 0; j < height; j++)
        for(int i = 0; i < width; i++)
                *(reserve_mem + i + j*800) = cur_lcd_buf[j][i];

    printf("当前在%s函数内，已成功拷贝a区的图片数据\n", __FUNCTION__);


    //nex图片合成像素点
    int nex_lcd_buf [height][width];

    for(int j = 0, J = 0; j < height; j++, J+=y_diff){
        for(int i = 0, I = 0; i < width; i++, I+=x_diff){
            nex_lcd_buf[height - j -1][i] = nex_rgb_buf[ (J*nex_info.width+I)*3 + 0 + J*pad_nex ] << 0  |   //B
                                            nex_rgb_buf[ (J*nex_info.width+I)*3 + 1 + J*pad_nex ] << 8  |   //G
                                            nex_rgb_buf[ (J*nex_info.width+I)*3 + 2 + J*pad_nex ] << 16 |   //R
                                            0x00 << 24;                                                //A
        }
    }
    //将nex图片显示在b区
    for(int j = 0, J = 480; j < height; j++, J++)
        for(int i = 0; i < width; i++)
                *(reserve_mem + i + J*800) = nex_lcd_buf[j][i];

    printf("当前在%s函数内，已成功拷贝b区的图片数据\n", __FUNCTION__);

    //pre图片合成像素点
    int pre_lcd_buf [height][width];

    for(int j = 0, J = 0; j < height; j++, J+=y_diff){
        for(int i = 0, I = 0; i < width; i++, I+=x_diff){
            pre_lcd_buf[height - j -1][i] = pre_rgb_buf[ (J*pre_info.width+I)*3 + 0 + J*pad_pre ] << 0  |   //B
                                            pre_rgb_buf[ (J*pre_info.width+I)*3 + 1 + J*pad_pre ] << 8  |   //G
                                            pre_rgb_buf[ (J*pre_info.width+I)*3 + 2 + J*pad_pre ] << 16 |   //R
                                            0x00 << 24;                                                //A
        }
    }
    //将pre图片显示在c区
    for(int j = 0, J = 960; j < height; j++, J++)
        for(int i = 0; i < width; i++)
                *(reserve_mem + i + J*800) = pre_lcd_buf[j][i];

    printf("当前在%s函数内，已成功拷贝c区的图片数据\n", __FUNCTION__);

}

#endif

#if !VERSION
//功  能: 将前后节点的图片数据写进虚拟显存的b、c区域
void prepare_node(P_node cur, unsigned int width, unsigned int height){

    //每当进入函数时，就证明需要切换图片
    change_bmp = true;

    P_node nex = get_next_node(cur, bmpList);
    P_node pre = get_prev_node(cur, bmpList);

    //打开三个节点的图片
    FILE * fp_cur = fopen(cur->Data.name, "r");
    FILE * fp_nex = fopen(nex->Data.name, "r");
    FILE * fp_pre = fopen(pre->Data.name, "r");

    struct bitmap_info cur_info;
    struct bitmap_info nex_info;
    struct bitmap_info pre_info;

    cur_info = read_bmp_head(fp_cur);
    nex_info = read_bmp_head(fp_nex);
    pre_info = read_bmp_head(fp_pre);

    int pad_cur = 0;
    int pad_nex = 0;
    int pad_pre = 0;

    if( (cur_info.width * 3) % 4 != 0 ) pad_cur = 4 - ((cur_info.width * 3) % 4);
    if( (nex_info.width * 3) % 4 != 0 ) pad_nex = 4 - ((nex_info.width * 3) % 4);
    if( (pre_info.width * 3) % 4 != 0 ) pad_pre = 4 - ((pre_info.width * 3) % 4);

    //开始读取当前图片像素点数据
    int rgb_size_cur = (cur_info.width * 3 + pad_cur) * cur_info.height;
    char cur_rgb_buf[rgb_size_cur];
    if( fread(cur_rgb_buf, rgb_size_cur, 1, fp_cur) != 1 ){
        perror("读取cur节点图片像素点数据失败!");
        // continue; 
    }
    int rgb_size_nex = (nex_info.width * 3 + pad_nex) * nex_info.height;
    char nex_rgb_buf[rgb_size_nex];
    if( fread(nex_rgb_buf, rgb_size_nex, 1, fp_nex) != 1 ){
        perror("读取nex节点图片像素点数据失败!");
        // continue; 
    }
    int rgb_size_pre = (pre_info.width * 3 + pad_pre) * pre_info.height;
    char pre_rgb_buf[rgb_size_pre];
    if( fread(pre_rgb_buf, rgb_size_pre, 1, fp_pre) != 1 ){
        perror("读取pre节点图片像素点数据失败!");
        // continue; 
    }


    //cur图片合成像素点
    int cur_lcd_buf [height][width];
    unsigned int x_diff = cur_info.width / width;   //计算宽度的每次偏移量
    unsigned int y_diff = cur_info.height / height; //计算高度的每次偏移量

    for(int j = 0, J = 0; j < height; j++, J+=y_diff){
        for(int i = 0, I = 0; i < width; i++, I+=x_diff){
            cur_lcd_buf[height - j -1][i] = cur_rgb_buf[ (J*cur_info.width+I)*3 + 0 + J*pad_cur ] << 0  |   //B
                                            cur_rgb_buf[ (J*cur_info.width+I)*3 + 1 + J*pad_cur ] << 8  |   //G
                                            cur_rgb_buf[ (J*cur_info.width+I)*3 + 2 + J*pad_cur ] << 16 |   //R
                                            0x00 << 24;                                                //A
        }
    }
    //将cur图片显示在中区
    for(int j = 0; j < height; j++)
        for(int i = 0; i < width; i++)
                *(reserve_mem + (i+800) + j*2400) = cur_lcd_buf[j][i];


    //nex图片合成像素点
    int nex_lcd_buf [height][width];

    for(int j = 0, J = 0; j < height; j++, J+=y_diff){
        for(int i = 0, I = 0; i < width; i++, I+=x_diff){
            nex_lcd_buf[height - j -1][i] = nex_rgb_buf[ (J*nex_info.width+I)*3 + 0 + J*pad_nex ] << 0  |   //B
                                            nex_rgb_buf[ (J*nex_info.width+I)*3 + 1 + J*pad_nex ] << 8  |   //G
                                            nex_rgb_buf[ (J*nex_info.width+I)*3 + 2 + J*pad_nex ] << 16 |   //R
                                            0x00 << 24;                                                //A
        }
    }
    //将nex图片显示在右区
    for(int j = 0; j < height; j++)
        for(int i = 0; i < width; i++)
                *(reserve_mem + (i+1600) + j*2400) = nex_lcd_buf[j][i];


    //pre图片合成像素点
    int pre_lcd_buf [height][width];

    for(int j = 0, J = 0; j < height; j++, J+=y_diff){
        for(int i = 0, I = 0; i < width; i++, I+=x_diff){
            pre_lcd_buf[height - j -1][i] = pre_rgb_buf[ (J*pre_info.width+I)*3 + 0 + J*pad_pre ] << 0  |   //B
                                            pre_rgb_buf[ (J*pre_info.width+I)*3 + 1 + J*pad_pre ] << 8  |   //G
                                            pre_rgb_buf[ (J*pre_info.width+I)*3 + 2 + J*pad_pre ] << 16 |   //R
                                            0x00 << 24;                                                //A
        }
    }
    //将pre图片显示在左区
    for(int j = 0; j < height; j++)
        for(int i = 0; i < width; i++)
                *(reserve_mem + i + j*2400) = pre_lcd_buf[j][i];
}

#endif

//功  能: 刷新相册里面的缩略图栏
void flush_state_line(P_node cur){

    P_node pre   = get_prev_node(cur, bmpList);
    P_node nex   = get_next_node(cur, bmpList);
    lcd_pos_size_pixel(cur, 200, 240, 400, 240);
    lcd_pos_size_pixel(cur, 300, 10, 200, 120);
    lcd_pos_size_pixel(pre, 150, 50, 100, 60);
    lcd_pos_size_pixel(nex, 550, 50, 100, 60);
}

//功  能: 定位显示图片，及分辨率
int * lcd_pos_size_pixel(P_node node, const unsigned int x, const unsigned int y, int width, int height){

    //每当进入函数时，就证明需要切换图片
    change_bmp = true;
    
    //打开图片  
    printf("当前需要打开的图片是---->%s\n",node->Data.name);
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
    int lcd_buf [height][width];
    unsigned int x_diff = bmp_info.width / width;   //计算宽度的每次偏移量
    unsigned int y_diff = bmp_info.height / height; //计算高度的每次偏移量

    for(int j = 0, J = 0; j < height; j++, J+=y_diff){
        for(int i = 0, I = 0; i < width; i++, I+=x_diff){
            lcd_buf[height - j -1][i] = rgb_buf[ (J*bmp_info.width+I)*3 + 0 + J*polish ] << 0  |   //B
                                        rgb_buf[ (J*bmp_info.width+I)*3 + 1 + J*polish ] << 8  |   //G
                                        rgb_buf[ (J*bmp_info.width+I)*3 + 2 + J*polish ] << 16 |   //R
                                        0x00 << 24;                                                //A
        }
    }
    //将图片显示在a区
    for(int j = 0; j < height; j++)
        for(int i = 0; i < width; i++)
                *(map + i+x + (j+y)*800) = lcd_buf[j][i];

    int * tmp = calloc(1, height * width * 4);
    memcpy(tmp, lcd_buf, height * width * 4);
    return tmp;
    // ioctl(fd_lcd, FBIOPAN_DISPLAY, &varinfo);

}


//功  能: 点击放大图片
P_node click2show(P_node cur){


    //初始化随机数种子
    srand((int)time(NULL));

    //先获取当前图片的信息，并且将数据放进a区
    unsigned long (*buf)[WIDTH] = calloc(1, SCREEN_SIZE * 4);
    get_image(cur, buf);

    //定义一个函数指针数组，随机抽取一个出场方式
    void (*present_fun_arr[4])() = {fall_down_in, blind_window_in, left2right_in, float_up_in};
    // blind_window_in(fd_lcd, buf);
    int fun_num = rand() % 3;
    //获取前一张图片以及下一张图片的数据
    prepare_node(cur, 800, 480);
    present_fun_arr[(int)(rand()%3)](fd_lcd, buf);


    while(1){
        tc_state.end_pos.x = 0;
        tc_state.end_pos.y = 0;
        if(pos_x > 0 && pos_x < 70 && pos_y > 0 && pos_y < 100){
            printf("退出放大!\n");
            pos_x = 0;
            pos_y = 0;
            break;
        }
        if(tc_state.start_pos.x < 400 && tc_state.x_ready && tc_state.y_ready && pos_buf.type == EV_SYN){
            slide_right2left();
            if(tc_state.end_pos.x > 400){
                //更新上一张图片并显示
                cur = get_prev_node(cur, bmpList);
                display_node(cur);
                prepare_node(cur, 800, 480);
                
            }
        }
        if(tc_state.start_pos.x > 400 && tc_state.x_ready && tc_state.y_ready && pos_buf.type == EV_SYN){
            slide_left2right();
            if(tc_state.end_pos.x < 400 && tc_state.end_pos.x != 0){
                //更新下一张图片并显示
                printf("当前的end_pos.x = %d\n", tc_state.end_pos.x);
                cur = get_next_node(cur, bmpList);
                display_node(cur);
                prepare_node(cur, 800, 480);
            }
        }   
    }
    return cur;
}

//功  能:显示下一张图片
P_node display_next_photo(P_node pos, P_node bmpList){

    P_node nex = get_next_node(pos, bmpList);
    // slide2change(1);
    prepare_node(pos, 400, 240);
    flush_state_line(nex);
    return nex;
}

//功  能:显示上一张图片
P_node display_prev_photo(P_node pos, P_node bmpList){

    P_node pre = get_prev_node(pos, bmpList);
    // slide2change(2);
    prepare_node(pos, 400, 240);
    flush_state_line(pre);
    return pre;
}