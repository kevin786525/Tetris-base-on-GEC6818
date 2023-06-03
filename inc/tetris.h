#ifndef _TETRIS_H_
#define _TETRIS_H_

#include "commonheader.h"
#include "bmp.h"
#include "touch.h"
#include "my_types.h"
#include "list.h"
#include "head4animation.h"
#include "crash.h"

//下降速度
#define ONE_S   1000000     //1秒
#define HUN_MS  100000      //100毫秒
#define ONE_MS  1000        //1毫秒
#define HUN_US  100         //100微秒

#define BLOCK_COUNT     7
#define WIDTH_PIX       5       //横向格子数
#define HEIGHT_PIX      5       //竖向格子数
#define CELL_PER_PIX    20      //每个格子占像素点数
#define GAME_WIDTH      320     //游戏宽度（像素点）
#define GAME_HEIGHT     600     //游戏高度（像素点）

#define RED     0x00ED1C24      //红
#define CYAN    0x000EF717      //青
#define BLUE    0x0021BCFA      //蓝
#define YELLOW  0x00E8FA12      //黄
#define ORANGE  0x00A349A4      //紫色
#define BLACK   0x00000000      //黑
#define WHITE   0xFFFFFFFF      //白
#define GRAY    0x00C3C3C3      //灰

#define TETRIS_DEBUG 0
#define PRINT_INFO   0
#define THREAD_DEBUG 0

typedef enum{
    CLEAN_BACK = 0,
    CLEAN_LEFT,
    CLEAN_RIGHT,
    CLEAN_RB,
    CLEAN_LB
}clean_mode;

typedef enum{
    BLOCK_I = 0,        //CYAN
    BLOCK_B,            //YELLOW
    BLOCK_T,            //RED
    BLOCK_RL,           //BLUE
    BLOCK_LL,
    BLOCK_RZ,           //ORANGE
    BLOCK_LZ
}block_category;


typedef enum {		//方块朝向
	UP = 0,
	RIGHT,
	LEFT,
	DOWN
}block_dir_t;
 
typedef enum {		//方块移动方向
	MOVE_DOWN = 0,
	MOVE_LEFT,
	MOVE_RIGHT
}move_dir_t;

typedef struct {
    unsigned long c_color;
    int data;

}Cell;

typedef struct block{
    block_category cate;
    block_dir_t dir;
    move_dir_t mov;
    int x;      //方块总成在全局的x坐标
    int y;      //方块总成在全局的y坐标
    // unsigned int center_point;
    unsigned long color;
    // int block_data[WIDTH_PIX*CELL_PER_PIX][HEIGHT_PIX*CELL_PER_PIX];
}Block, *P_block;



//俄罗斯方块游戏
void play_tetris(P_node head);


void tetris_game(P_node head);

//设置方块的数据
P_block set_Data();

//保持方块下降
void  down_block(void);

//随机生成方块
void rand_block_data(P_block new); 

//固化方块数据
void solidify_data(void);

//查看背景数据
void read_back_block();

//查看方块的数据
void read_block_data();

//检查是否需要消行
void check();

//消除某行方块，并且从上往下降落方块
void down(int x);

//检查游戏是否结束
bool failed_game();

#endif