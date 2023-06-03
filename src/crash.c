#include "crash.h"

//============================全局变量定义============================//

//由于线程原因，因此不得不定义一个共享资源定位全局变量
extern Block g_block;
extern int block[BLOCK_COUNT * 4][WIDTH_PIX][HEIGHT_PIX];
extern int back_block[GAME_HEIGHT/CELL_PER_PIX][GAME_WIDTH/CELL_PER_PIX];

extern pthread_mutex_t lock;


//============================全局变量定义============================//


//根据方块的x, y值判断是否要准备与墙发生碰撞
bool is_downable(){
    int id = g_block.cate*4+g_block.dir;
    //首先计算方块总成在背景格子中的第几行第几列
    int row  = g_block.y;
    int line = g_block.x;
    for(int i = 0; i < WIDTH_PIX; i++){
        for(int j = 0; j < HEIGHT_PIX; j++){
            if(block[id][i][j] == 1 && (back_block[row+j+1][line+i] == 1) ){
                //一旦产生了下降碰撞，就固化数据
                solidify_data();
                read_back_block();
                return false;
            }
        }
    }
    return true;
}

//判断 是否与右墙以及右方块发生碰撞事件
bool is_R_moveable(){
    int id   = g_block.cate*4+g_block.dir;
    int row  = g_block.y;
    int line = g_block.x;
    for(int i = 0; i < WIDTH_PIX; i++){
        for(int j = 0; j < HEIGHT_PIX; j++){
            // printf("block[%d][%d][%d]:%d\n", id, i, j, block[id][i][j]);
            // printf("back_block[%d][%d]:%d\n", row+j, line+i, back_block[row+j][line+i]);
            if(block[id][i][j] == 1 && (back_block[row+j][line+i] == 1) ){
                pthread_mutex_lock(&lock);
                g_block.x--;
                pthread_mutex_unlock(&lock);
                return true;
            }
        }
    }
    return true;
}

//判断 是否与右墙以及右方块发生碰撞事件
bool is_L_moveable(){
    int id   = g_block.cate*4+g_block.dir;
    int row  = g_block.y;
    int line = g_block.x;
    for(int i = 0; i < WIDTH_PIX; i++){
        for(int j = 0; j < HEIGHT_PIX; j++){
            if(block[id][i][j] == 1 && (back_block[row+j][line+i] == 1) ){
                pthread_mutex_lock(&lock);
                g_block.x++;
                pthread_mutex_unlock(&lock);
                return true;
            }
        }
    }
    return true;
}

//判断是否可以旋转
bool handle_rotate(){
    // if(!is_downable()){
    //     return false;
    // }
    int id   = g_block.cate*4+g_block.dir;
    int row  = g_block.y;
    int line = g_block.x;
    for(int i = 0; i < WIDTH_PIX; i++){
        for(int j = 0; j < HEIGHT_PIX; j++){
            //如果是旋转过后与墙发生重叠，则回退一格
            if(block[id][i][j] == 1 && ((back_block[row+j][line+i] == 1)) && g_block.x <= 0 ){
                // printf("不能旋转...\n");
                pthread_mutex_lock(&lock);
                g_block.x++;
                pthread_mutex_unlock(&lock);
                return true;
            }else if(block[id][i][j] == 1 && ((back_block[row+j][line+i] == 1)) && g_block.x >= 9){
                pthread_mutex_lock(&lock);
                g_block.x--;
                pthread_mutex_unlock(&lock);
                return true;
            }else if(block[id][i][j] == 1 && (back_block[row+j][line+i] == 1)){
                return false;
            }
        }
    }
    return true;
}   


//判断是否可以旋转
bool is_rotate(){
    int id   = g_block.cate*4+g_block.dir;
    int row  = g_block.y;
    int line = g_block.x;
    for(int i = 0; i < WIDTH_PIX; i++){
        for(int j = 0; j < HEIGHT_PIX; j++){
            if(block[id][i][j] == 1 && (back_block[row+j][line+i] == 1)){

            }
        }
    }
}
