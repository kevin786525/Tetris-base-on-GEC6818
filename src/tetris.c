#include "tetris.h"

//============================全局变量定义============================//

//由于线程原因，因此不得不定义一个共享资源定位全局变量
Block g_block;
Block n_block;
Block v_block;

//伪状态机
extern bool change_bmp;
extern bool is_playing_video;


extern int fd_lcd;
extern int * map;

extern unsigned long pos_x;
extern unsigned long pos_y;


//获取屏幕的固定属性
extern struct fb_fix_screeninfo fixinfo;
extern struct fb_var_screeninfo varinfo; // 可变属性
extern struct touch_state tc_state;
extern struct input_event pos_buf;


int block[BLOCK_COUNT * 4][WIDTH_PIX][HEIGHT_PIX] = {
    // I 方块  第一列       第二列             第三列           第四列           第五列
    {{0, 0, 0, 0, 0},  {0, 0, 0, 0, 0},  {0, 1, 1, 1, 0},  {0, 0, 0, 0, 0},  {0, 0, 0, 0, 0}},  //up
    {{0, 0, 0, 0, 0},  {0, 0, 1, 0, 0},  {0, 0, 1, 0, 0},  {0, 0, 1, 0, 0},  {0, 0, 0, 0, 0}},  //right
    {{0, 0, 0, 0, 0},  {0, 0, 0, 0, 0},  {0, 1, 1, 1, 0},  {0, 0, 0, 0, 0},  {0, 0, 0, 0, 0}},  //down
    {{0, 0, 0, 0, 0},  {0, 0, 1, 0, 0},  {0, 0, 1, 0, 0},  {0, 0, 1, 0, 0},  {0, 0, 0, 0, 0}},  //left
    
    // 田 方块             第二列             第三列           第四列           第五列
    {{0, 0, 0, 0, 0},  {0, 1, 1, 0, 0},  {0, 1, 1, 0, 0},  {0, 0, 0, 0, 0},  {0, 0, 0, 0, 0}},  //up
    {{0, 0, 0, 0, 0},  {0, 1, 1, 0, 0},  {0, 1, 1, 0, 0},  {0, 0, 0, 0, 0},  {0, 0, 0, 0, 0}},  //right
    {{0, 0, 0, 0, 0},  {0, 1, 1, 0, 0},  {0, 1, 1, 0, 0},  {0, 0, 0, 0, 0},  {0, 0, 0, 0, 0}},  //down
    {{0, 0, 0, 0, 0},  {0, 1, 1, 0, 0},  {0, 1, 1, 0, 0},  {0, 0, 0, 0, 0},  {0, 0, 0, 0, 0}},  //left

    // T 方块              第二列             第三列           第四列           第五列
    {{0, 0, 0, 0, 0},  {0, 0, 1, 0, 0},  {0, 1, 1, 0, 0},  {0, 0, 1, 0, 0},  {0, 0, 0, 0, 0}},  //up
    {{0, 0, 0, 0, 0},  {0, 0, 0, 0, 0},  {0, 1, 1, 1, 0},  {0, 0, 1, 0, 0},  {0, 0, 0, 0, 0}},  //right
    {{0, 0, 0, 0, 0},  {0, 0, 1, 0, 0},  {0, 0, 1, 1, 0},  {0, 0, 1, 0, 0},  {0, 0, 0, 0, 0}},  //down
    {{0, 0, 0, 0, 0},  {0, 0, 1, 0, 0},  {0, 1, 1, 1, 0},  {0, 0, 0, 0, 0},  {0, 0, 0, 0, 0}},  //left
    
    // L 方块(RL)         第二列             第三列           第四列           第五列
    {{0, 0, 0, 0, 0},  {0, 0, 0, 0, 0},  {0, 1, 1, 1, 0},  {0, 0, 0, 1, 0},  {0, 0, 0, 0, 0}},  //up
    {{0, 0, 0, 0, 0},  {0, 0, 1, 1, 0},  {0, 0, 1, 0, 0},  {0, 0, 1, 0, 0},  {0, 0, 0, 0, 0}},  //right
    {{0, 0, 0, 0, 0},  {0, 1, 0, 0, 0},  {0, 1, 1, 1, 0},  {0, 0, 0, 0, 0},  {0, 0, 0, 0, 0}},  //down
    {{0, 0, 0, 0, 0},  {0, 0, 1, 0, 0},  {0, 0, 1, 0, 0},  {0, 1, 1, 0, 0},  {0, 0, 0, 0, 0}},  //left

    // L 方块(LL)         第二列             第三列           第四列           第五列 
    {{0, 0, 0, 0, 0},  {0, 1, 1, 0, 0},  {0, 0, 1, 0, 0},  {0, 0, 1, 0, 0},  {0, 0, 0, 0, 0}},  //up
    {{0, 0, 0, 0, 0},  {0, 0, 0, 0, 0},  {0, 1, 1, 1, 0},  {0, 1, 0, 0, 0},  {0, 0, 0, 0, 0}},  //right
    {{0, 0, 0, 0, 0},  {0, 0, 1, 0, 0},  {0, 0, 1, 0, 0},  {0, 0, 1, 1, 0},  {0, 0, 0, 0, 0}},  //down
    {{0, 0, 0, 0, 0},  {0, 0, 0, 1, 0},  {0, 1, 1, 1, 0},  {0, 0, 0, 0, 0},  {0, 0, 0, 0, 0}},  //left

    // Z 方块(RZ)         第二列             第三列           第四列           第五列
    {{0, 0, 0, 0, 0},  {0, 1, 0, 0, 0},  {0, 1, 1, 0, 0},  {0, 0, 1, 0, 0},  {0, 0, 0, 0, 0}},  //up
    {{0, 0, 0, 0, 0},  {0, 0, 1, 1, 0},  {0, 1, 1, 0, 0},  {0, 0, 0, 0, 0},  {0, 0, 0, 0, 0}},  //right
    {{0, 0, 0, 0, 0},  {0, 1, 0, 0, 0},  {0, 1, 1, 0, 0},  {0, 0, 1, 0, 0},  {0, 0, 0, 0, 0}},  //down
    {{0, 0, 0, 0, 0},  {0, 0, 1, 1, 0},  {0, 1, 1, 0, 0},  {0, 0, 0, 0, 0},  {0, 0, 0, 0, 0}},  //left

    // Z 方块(LZ)         第二列             第三列           第四列           第五列
    {{0, 0, 0, 0, 0},  {0, 0, 1, 0, 0},  {0, 1, 1, 0, 0},  {0, 1, 0, 0, 0},  {0, 0, 0, 0, 0}},  //up
    {{0, 0, 0, 0, 0},  {0, 0, 0, 0, 0},  {0, 1, 1, 0, 0},  {0, 0, 1, 1, 0},  {0, 0, 0, 0, 0}},  //right
    {{0, 0, 0, 0, 0},  {0, 0, 1, 0, 0},  {0, 1, 1, 0, 0},  {0, 1, 0, 0, 0},  {0, 0, 0, 0, 0}},  //down
    {{0, 0, 0, 0, 0},  {0, 0, 0, 0, 0},  {0, 1, 1, 0, 0},  {0, 0, 1, 1, 0},  {0, 0, 0, 0, 0}},  //left

};

int back_block[GAME_HEIGHT/CELL_PER_PIX][GAME_WIDTH/CELL_PER_PIX];  //背景数据
int markColor[GAME_HEIGHT/CELL_PER_PIX][GAME_WIDTH/CELL_PER_PIX];   //背景颜色数据

int Color[7] = {RED, CYAN, BLUE, YELLOW, ORANGE};
char Cate[7][10]  = {"I型", "田字型", "T字型", "RL型", "LL型", "RZ型", "LZ型"};
char Dir[4][10]   = {"上", "右", "下", "左"};

pthread_mutex_t lock;
pthread_cond_t cond;
P_node Start;
P_node Pause;

volatile bool pause_flag = false;
volatile bool reset_game = false;
volatile bool start_game = true;


//============================全局变量定义============================//


//让指定格子填充颜色
void __set_cell_color(int x, int y, int color){
    /* for(int i = 1; i < WIDTH_PIX*4; i++){
        for(int j = 1; j < HEIGHT_PIX*4; j++){
            *(tmp + (HEIGHT-((x+1)*WIDTH_PIX)*4 + i)*WIDTH + y*HEIGHT_PIX*4 + j) = color;
        }
    } */
    int h = HEIGHT_PIX*4;
    int w = WIDTH_PIX*4;
    for(int i = 0; i < w; i++){
        for(int j = 0; j < h; j++){
            if(i == 0 || j == 0 || i == w-1 || j == h-1){
                continue;
            }else{
                *(map + (HEIGHT-((x+1)*WIDTH_PIX)*4 + i)*WIDTH + y*HEIGHT_PIX*4 + j) = color;
            }
        }
    }
}

//画虚拟格子
void __set_v_cell(int x, int y, int color){
    int h = HEIGHT_PIX*4;
    int w = WIDTH_PIX*4;
    for(int i = 0; i < w; i++){
        for(int j = 0; j < h; j++){
            if(i == 0 || j == 0 || i == w-1 || j == h-1){
                *(map + (HEIGHT-((x+1)*WIDTH_PIX)*4 + i)*WIDTH + y*HEIGHT_PIX*4 + j) = color;
            }else{
                continue;
            }
        }
    }
}



//线程函数，让虚拟格子一直保持复制真是格子的状态
void * __copy_block(void * args){
    while(1){
        prev_place();
    }
}

//设置墙壁数据
void __set_wall_data(void){
    int back_width = GAME_WIDTH/CELL_PER_PIX;
    int back_height = GAME_HEIGHT/CELL_PER_PIX;
    for(int i = 0; i < back_height; i++){
        for(int j = 0; j < back_width; j++)
            if( j == 0 || j == back_width-1 || i == back_height-1){
                back_block[i][j] = 1;
            }else{
                back_block[i][j] = 0;
            }
    }
}

//画出游戏墙壁
void __draw_wall(void){
    for(int i = 0; i < GAME_HEIGHT/CELL_PER_PIX; i++){
        for(int j = 0; j < GAME_WIDTH/CELL_PER_PIX; j ++){
            usleep(100);
            if(back_block[i][j]){
                __set_cell_color(j, i, GRAY);
                markColor[i][j] = GRAY;
            }else{
                __set_cell_color(j, i, BLACK);
                __set_v_cell(j, i, BLACK);
                markColor[i][j] = BLACK;
            }
        }
    }
}

void read_block_data(){
    printf("方块的种类:%s\n方块的方向:%s\n方块的x坐标:%d\n方块的y坐标:%d\n", Cate[g_block.cate], Dir[g_block.dir], g_block.x, g_block.y);
}

//查看背景数据
void read_back_block(){
    for(int i = 0; i < GAME_HEIGHT/CELL_PER_PIX; i++){
        for(int j = 0; j < GAME_WIDTH/CELL_PER_PIX; j++){
            printf("%d ", back_block[i][j]);
        }
        printf("\n");
    }
}



//画虚拟格子总成
void __draw_v_block(){
    int id = g_block.cate*4+g_block.dir;
    for(int i = 0; i < HEIGHT_PIX; i++){
        for(int j = 0; j < WIDTH_PIX; j++){
            if(!block[id][i][j]){
                continue;
            }else{
                __set_v_cell(v_block.x+i, v_block.y+j, WHITE);
            }
        }
    }
}

//画方块总成
void __draw_block(int cate, int dir){
    for(int i = 0; i < HEIGHT_PIX; i++){
        for(int j = 0; j < WIDTH_PIX; j++){
            if(!block[cate*4+dir][i][j]){
                continue;
            }else{
                __set_cell_color(g_block.x+i, g_block.y+j, g_block.u32);
            }
        }
    }
}



//画出下一个格子在指定位置
void set_n_block(){
    int n_id = n_block.cate*4+n_block.dir;
    for(int i = 0; i < HEIGHT_PIX; i++){
            for(int j = 0; j < WIDTH_PIX; j++){
                if(!block[n_id][i][j]){
                    continue;
                }else{
                    __set_cell_color(18+i, 3+j, n_block.u32);
                }
            }
        }
}

//清理痕迹
void __clean_mark(){
    for(int i = 1; i < HEIGHT_PIX; i++){
            for(int j = 1; j < WIDTH_PIX; j++){
                if(!block[g_block.cate*4+g_block.dir][i][j]){
                    continue;
                }else{
                    __set_cell_color(g_block.x+i, g_block.y+j, BLACK);
                }
            }
        }
}

//清理虚拟方块的痕迹
void __clean_v_mark(){
    for(int i = 1; i < HEIGHT_PIX; i++){
            for(int j = 1; j < WIDTH_PIX; j++){
                if(!block[v_block.cate*4+v_block.dir][i][j]){
                    continue;
                }else{
                    __set_v_cell(v_block.x+i, v_block.y+j, BLACK);
                }
            }
        }
}

//清理下一块的痕迹
void __clean_n_Mark(){
    for(int i = 1; i < HEIGHT_PIX; i++){
            for(int j = 1; j < WIDTH_PIX; j++){
                if(!block[g_block.cate*4+g_block.dir][i][j]){
                    continue;
                }else{
                    __set_cell_color(18+i, 3+j, BLACK);
                }
            }
        }
}


//线程函数: 保持方块下降
void * __down_block(void * args){
    // Block t = *(Block *)args;
    Block t;
    t.cate = BLOCK_T;
    t.dir = UP;
    t.u32 = ORANGE;
    t.x = 1;
    t.y = 1;
    set_Data(t);
    while(t.x  > 1){
        t.x--;
        usleep(1000000);
        set_Data(t);
        __clean_mark(t, CLEAN_RIGHT);
    }
}

void __pthread_down_block(void){
    pthread_t b_tid;
    pthread_mutex_init(&lock, NULL);
    // pthread_create(&b_tid, NULL, down_block, (void *)&t);
    pthread_create(&b_tid, NULL, __down_block, NULL);
    if( pthread_detach(b_tid) != 0){
        perror("线程分离失败:");
    }
}

void play_tetris(P_node head){
    unsigned long (*buf)[WIDTH] = calloc(1, SCREEN_SIZE * 4);
    P_node tetris_start = search_2_list(head, "tetris_start");
    get_image(tetris_start, buf);
    while(1){
        blind_window_in(fd_lcd, buf);
        if(pos_x > 30 && pos_x < 135 && pos_y > 180 && pos_y < 330){        //开始游戏
            pos_x = 0;
            pos_y = 0;
            start_game = true;
            printf("开始游戏\n");
            tetris_game(head);
        }
        if(pos_x > 160 && pos_x < 260 && pos_y > 180 && pos_y < 330){        //历史成绩
            pos_x = 0;
            pos_y = 0;
        }
        if(pos_x > 285 && pos_x < 385 && pos_y > 180 && pos_y < 330){        //游戏说明
            pos_x = 0;
            pos_y = 0;
        }
        if(pos_x > 430 && pos_x < 530 && pos_y > 180 && pos_y < 330){        //退出游戏
            pos_x = 0;
            pos_y = 0;
        }
        if(pos_x > 570 && pos_x < 670 && pos_y > 180 && pos_y < 330){        //隐藏福利
            pos_x = 0;
            pos_y = 0;
        }
        
    }
}



#if !TETRIS_DEBUG
// 俄罗斯方块游戏
void tetris_game(P_node head){
    unsigned long (*buf)[WIDTH] = calloc(1, SCREEN_SIZE * 4);
    P_node tetris_back = search_2_list(head, "tetris_back");
    get_image(tetris_back, buf);
    Pause  = search_2_list(head, "mypause");
    Start  = search_2_list(head, "mystart");
    blind_window_in(fd_lcd, buf);
    lcd_pos_size_pixel(Pause, 250, 30, 100, 100);

    //设置随机种子
    srand((unsigned)time(NULL));

    //设置墙壁数据
    __set_wall_data();
    //画出游戏墙壁
    __draw_wall();
#if THREAD_DEBUG
    __pthread_down_block(); 
#endif

    down_block(head);
    free(buf);
}
#endif



//设置方块的数据
P_block set_Data(){
    
    //首先判断是哪种方块,那种方向,然后找到对应的数据
    switch(g_block.cate){
        case BLOCK_I:
            switch(g_block.dir){
                case UP:
                    __draw_block(BLOCK_I, UP);
                    break;

                case RIGHT:
                    __draw_block(BLOCK_I, RIGHT);
                    break;
                
                case DOWN:
                    __draw_block(BLOCK_I, DOWN);
                    break;
                
                case LEFT:
                    __draw_block(BLOCK_I, LEFT);
                    break;
            }
            break;
        
        case BLOCK_B:
            switch(g_block.dir){
                case UP:
                    __draw_block(BLOCK_B, UP);
                    break;

                case RIGHT:
                    __draw_block(BLOCK_B, RIGHT);
                    break;
                
                case DOWN:
                    __draw_block(BLOCK_B, DOWN);
                    break;
                
                case LEFT:
                    __draw_block(BLOCK_B, LEFT);
                    break;
                }
            break;
        
        case BLOCK_T:
            switch(g_block.dir){
                case UP:
                    __draw_block(BLOCK_T, UP);
                    break;

                case RIGHT:
                    __draw_block(BLOCK_T, RIGHT);
                    break;
                
                case DOWN:
                    __draw_block(BLOCK_T, DOWN);
                    break;
                
                case LEFT:
                    __draw_block(BLOCK_T, LEFT);
                    break;
            }
            break;
        
        case BLOCK_RL:
            switch(g_block.dir){
                case UP:
                    __draw_block(BLOCK_RL, UP);
                    break;

                case RIGHT:
                    __draw_block(BLOCK_RL, RIGHT);
                    break;
                
                case DOWN:
                    __draw_block(BLOCK_RL, DOWN);
                    break;
                
                case LEFT:
                    __draw_block(BLOCK_RL, LEFT);
                    break;
            }
            break;
        
        case BLOCK_LL:
            switch(g_block.dir){
                case UP:
                    __draw_block(BLOCK_LL, UP);
                    break;

                case RIGHT:
                    __draw_block(BLOCK_LL, RIGHT);
                    break;
                
                case DOWN:
                    __draw_block(BLOCK_LL, DOWN);
                    break;
                
                case LEFT:
                    __draw_block(BLOCK_LL, LEFT);
                    break;
            }
            break;

        case BLOCK_RZ:
            switch(g_block.dir){
                case UP:
                    __draw_block(BLOCK_RZ, UP);
                    break;

                case RIGHT:
                    __draw_block(BLOCK_RZ, RIGHT);
                    break;
                
                case DOWN:
                    __draw_block(BLOCK_RZ, DOWN);
                    break;
                
                case LEFT:
                    __draw_block(BLOCK_RZ, LEFT);
                    break;
            }
            break;
        
        case BLOCK_LZ:
            switch(g_block.dir){
                case UP:
                    __draw_block(BLOCK_LZ, UP);
                    break;

                case RIGHT:
                    __draw_block(BLOCK_LZ, RIGHT);
                    break;
                
                case DOWN:
                    __draw_block(BLOCK_LZ, DOWN);
                    break;
                
                case LEFT:
                    __draw_block(BLOCK_LZ, LEFT);
                    break;
            }
            break;

        default:
            break;
    }
}



void * __option_block (void * args){
    
    while(1){
        if(pos_x > 390 && pos_x < 450 && pos_y > 10 && pos_y < 110){        //重启游戏
            pos_x = 0;
            pos_y = 0;
            printf("重启游戏\n");
            reset_game = true;
            pthread_exit(NULL);                 //关闭本次线程
        }
        if(pos_x > 250 && pos_x < 350 && pos_y > 10 && pos_y < 140){        //游戏暂停
            pos_x = 0;
            pos_y = 0;
            printf("游戏暂停...\n");
            pause_flag = true;
            lcd_pos_size_pixel(Start, 250, 30, 100, 100);
            pthread_exit(NULL);                 //游戏暂停之后退出本条线程
        }
        if(pos_x > 560 && pos_x < 640 && pos_y > 0 && pos_y < 70){          //右移方块
            printf("右移方块\n");
            __clean_mark();                     //先清除方块痕迹
            __clean_v_mark();
            g_block.x++;                        //再按照控制方向更新方块x，y数据
            if(is_R_moveable()){                //更新方块数据之后先判断是否会发生碰撞，如果发生了碰撞事件，则不进行数据更新操作
                pthread_mutex_lock(&lock);      //如果不发生碰撞事件，则更新放块
                set_Data(g_block);
                pthread_mutex_unlock(&lock);
                memcpy(&v_block, &g_block, sizeof(Block));
                prev_place();
                pos_x = 0;
                pos_y = 0;
            }else{
                // g_block.x--;
                pos_x = 0;
                pos_y = 0;
                continue;
            }
        }
        if(pos_x > 560 && pos_x < 640 && pos_y > 70 && pos_y < 150){        //左移方块
            printf("左移方块\n");
            __clean_mark();             //先清除方块痕迹
            __clean_v_mark();
            g_block.x--;                //再按照控制方向更新方块x，y数据
            if(is_L_moveable()){        //更新方块数据之后先判断是否会发生碰撞，如果发生了碰撞事件，则不进行数据更新操作
                pthread_mutex_lock(&lock);
                set_Data(g_block);
                pthread_mutex_unlock(&lock);
                memcpy(&v_block, &g_block, sizeof(Block));
                prev_place();
                pos_x = 0;
                pos_y = 0;
            }else{
                pos_x = 0;
                pos_y = 0;
                continue;
            }
        }
        if(pos_x > 457 && pos_x < 550 && pos_y > 0 && pos_y < 70){      //右转方块
            printf("右转了方块\n");
            __clean_mark();
            __clean_v_mark();
            g_block.dir += 1;
            if(g_block.dir == 4)    g_block.dir = 0;
            if(handle_rotate()){
                pthread_mutex_lock(&lock);
                set_Data(g_block);
                pthread_mutex_unlock(&lock);
                memcpy(&v_block, &g_block, sizeof(Block));
                prev_place();
                pos_x = 0;
                pos_y = 0;
            }else{
                g_block.dir -= 1;
                if(g_block.dir == -1)   g_block.dir = 3;
                pos_x = 0;
                pos_y = 0;
                continue;
            }
        }
        if(pos_x > 457 && pos_x < 550 && pos_y > 70 && pos_y < 150){        //左转方块
            printf("左转了方块\n");
            __clean_mark();
            __clean_v_mark();
            g_block.dir -= 1;
            if(g_block.dir == -1)   g_block.dir = 3;
            if(handle_rotate()){
                pthread_mutex_lock(&lock);
                set_Data(g_block);
                pthread_mutex_unlock(&lock);
                memcpy(&v_block, &g_block, sizeof(Block));
                prev_place();
                pos_x = 0;
                pos_y = 0;
            }else{
                g_block.dir += 1;
                if(g_block.dir == 4)   g_block.dir = 0;
                pos_x = 0;
                pos_y = 0;
                continue;
            }
        }
        if(pos_x > 645 && pos_x < 730 && pos_y > 30 && pos_y < 110){        //速降方块
            pos_x = 0;
            pos_y = 0;
            printf("速降方块\n");
            pthread_mutex_lock(&lock);
            __clean_mark();
            memcpy(&g_block, &v_block, sizeof(Block));
            set_Data();
            pthread_mutex_unlock(&lock);
        }
        if(pos_x > 740 && pos_x < 800 && pos_y > 0 && pos_y < 80){         //关闭音乐
            pos_x = 0;
            pos_y = 0;
            printf("关闭音乐\n");
        }
        if(pos_x > 740 && pos_x < 800 && pos_y > 90 && pos_y < 145){         //退出游戏
            pos_x = 0;
            pos_y = 0;
            printf("退出游戏\n");
            start_game = false;
            //退出游戏时结束本次线程
            pthread_exit(NULL);
        }
    }
}


//保持方块下降
void  down_block(P_node head){

    P_node con = search_2_list(head, "confirm_reset");
    pthread_t op_tid;       //操作线程
    pthread_t cp_tid;       //虚拟方块复制线程
    pthread_t rs_tid;
    pthread_create(&op_tid, NULL, __option_block, NULL);
    pthread_detach(op_tid);

    // pthread_create(&rs_tid, NULL, __copy_block, NULL);
    // pthread_detach(rs_tid);

    memset(&n_block, 0, sizeof(Block));
    rand_block_data(&n_block);
    while(start_game){
        //判断是否重启游戏
        if(reset_game){
            int * tmp =  calloc(1, HEIGHT*WIDTH*4); //用于暂存游戏的所有背景数据
            memcpy(tmp, map, HEIGHT*WIDTH*4);
            lcd_pos_size_pixel(con, 135, 185, 400, 240);
            pause_flag = true;                                                      //先暂停游戏
            while(1){
                if(pos_x > 330 && pos_x < 365 && pos_y > 310 && pos_y < 415){       //确认
                    pos_x = 0;
                    pos_y = 0;
                    pthread_create(&op_tid, NULL, __option_block, NULL);            //重新开始游戏时重新打开线程
                    pthread_detach(op_tid);
                    memcpy(map, tmp, HEIGHT*WIDTH*4);                               //将原本的数据画面还原
                    //设置墙壁数据
                    __set_wall_data();
                    //画出游戏墙壁
                    __draw_wall();
                    reset_game = false;
                    pause_flag = false;
                    break;
                }
                if(pos_x > 330 && pos_x < 365 && pos_y > 205 && pos_y < 300){       //取消
                    pos_x = 0;
                    pos_y = 0;
                    memcpy(map, tmp, HEIGHT*WIDTH*4);                               //将原本的数据画面还原
                    pthread_create(&op_tid, NULL, __option_block, NULL);            //重新开始游戏时重新打开线程
                    pthread_detach(op_tid);
                    reset_game = false;
                    pause_flag = false;
                    goto CONT;
                    break;
                }        
            }
            free(tmp);
        }
        //判断是否暂停游戏
        if(pause_flag){
            while(1){
                if(pos_x > 250 && pos_x < 350 && pos_y > 10 && pos_y < 110){    //开始游戏
                    printf("游戏开始!\n");
                    lcd_pos_size_pixel(Pause, 250, 30, 100, 100);
                    pos_x = 0;
                    pos_y = 0;  
                    pause_flag = false;
                    pthread_create(&op_tid, NULL, __option_block, NULL);    //重新开始游戏时重新打开线程
                    pthread_detach(op_tid);
                    goto CONT;
                }
                usleep(100);
            }
        }

        //拷贝下一开方块的数据
        memcpy(&g_block, &n_block, sizeof(Block));
        memcpy(&v_block, &g_block, sizeof(Block));
        //拷贝完成后更新下一块方块
        memset(&n_block, 0, sizeof(Block));
        rand_block_data(&n_block);
        __clean_n_Mark();
        set_n_block();              //将下一块方块显示在指定位置

        read_block_data();

        check();                //每次产生新方块之前都判断一下最后一行方块是否需要消除
        if(failed_game()){
            break;
        }
        while(is_downable() && handle_rotate() && !pause_flag && !reset_game && start_game){
            //主线程保持方块下落
CONT:
            __clean_mark();
            __clean_v_mark();
            g_block.y++;
            pthread_mutex_lock(&lock);
            set_Data(g_block);
            pthread_mutex_unlock(&lock);
            prev_place();

            usleep(500*ONE_MS);     //方块下降速度  300ms
        }
        
    }
    return ;
}

//预判下落位置
void prev_place(){
    // v_block.y = 0;
    int id = v_block.cate*4 + v_block.dir;
    int back_width = GAME_WIDTH/CELL_PER_PIX;       //横格子数
    int back_height = GAME_HEIGHT/CELL_PER_PIX-5;     //竖格子数
    while(v_is_downable()){
        v_block.y++;
    }
    __draw_v_block();
}

//随机生成方块
void rand_block_data(P_block new){
    new->cate = rand() % 7;
    new->dir  = rand() % 4;
    // new->u32  = Color[rand() % 5];
    new->x    = rand() % 10;
    new->y    = 0;
    switch(new->cate){
        case BLOCK_I:       new->u32  = CYAN;
            break;
        
        case BLOCK_B:       new->u32  = YELLOW;
            break;
        
        case BLOCK_T:       new->u32  = RED;
            break;
        
        case BLOCK_RL:      new->u32  = BLUE;
            break;

        case BLOCK_LL:      new->u32  = BLUE;
            break;

        case BLOCK_RZ:      new->u32  = ORANGE;
            break;
        
        case BLOCK_LZ:      new->u32  = ORANGE;
            break;

        default:
            break;
    }
}


//固化方块数据
void solidify_data(void){
    int back_height = GAME_HEIGHT / CELL_PER_PIX;
    int back_width  = GAME_WIDTH  / CELL_PER_PIX;
    int id = g_block.cate*4 + g_block.dir;
    for(int i = 0; i < WIDTH_PIX; i++){       //背景格子的行数
        for(int j = 0; j < HEIGHT_PIX; j++){    //背景格子的列数
            if(block[id][i][j]){
                back_block[g_block.y+j][g_block.x+i] = 1;
                markColor[g_block.y+j][g_block.x+i] = g_block.u32;
            }else{
                continue;
            }
        }
    } 
}


//检查游戏是否结束
bool failed_game(){
    if(g_block.y == 0 && !is_downable()){
        // set_Data();
        printf("游戏结束!\n");
        return true;
    }
    return false;
}

//检查是否需要消行
void check(){
    
    for(int i = GAME_HEIGHT/CELL_PER_PIX-2; i >= 0; i--){
        for(int j = 0; j < GAME_WIDTH/CELL_PER_PIX && back_block[i][j]; j++){
            if(j >= GAME_WIDTH/CELL_PER_PIX-1){
                printf("%d行数据需要清理\n", i);
                down(i);
                i++;
            }
        }
    }
}


//消除某行方块，并且从上往下降落方块
void down(int x){
    for(int i = x; i > 0; i--){
        for(int j = 0; j < GAME_WIDTH/CELL_PER_PIX; j++){
            if(back_block[i-1][j] == 1){
                back_block[i][j] = 1;
                markColor[i][j] = markColor[i-1][j];
                __set_cell_color(j, i,markColor[i][j]);
            }else{
                back_block[i][j] = 0;
                __set_cell_color(j, i,BLACK);
            }
        }
    }
}


