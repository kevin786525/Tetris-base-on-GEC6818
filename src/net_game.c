#include "net_game.h"

//============================全局变量定义============================//


//由于线程原因，因此不得不定义一个共享资源定位全局变量
extern Block g_block;          //当前方块
extern Block n_block;          //下一块方块
extern Block v_block;          //预判方块

//伪状态机
extern bool change_bmp;             //
extern bool is_playing_video;       //标志状态栏（不显示）


extern int fd_lcd;
extern int * map;

extern unsigned long pos_x;
extern unsigned long pos_y;


//获取屏幕的固定属性
extern struct fb_fix_screeninfo fixinfo;
extern struct fb_var_screeninfo varinfo; // 可变属性
extern struct touch_state tc_state;
extern struct input_event pos_buf;


extern int block[BLOCK_COUNT * 4][WIDTH_PIX][HEIGHT_PIX];

extern int back_block[GAME_HEIGHT/CELL_PER_PIX][GAME_WIDTH/CELL_PER_PIX];  //背景数据
extern int markColor[GAME_HEIGHT/CELL_PER_PIX][GAME_WIDTH/CELL_PER_PIX];   //背景颜色数据


extern pthread_mutex_t lock;
extern pthread_cond_t cond;

extern volatile bool start_game;        //标志游戏启动
extern volatile int speed;               //方块下降的默认初始速度

extern  int music_flag;              //播放音乐标志位 
extern  int score;



//============================全局变量定义============================//

void net_tetris(){

}

void login(P_node head){
    P_node login = search_2_list(head, "login_in");
    display_node(login);
    printf("请按顺序输入 <用户名> <ip地址>\n");

    char name[STR_LEN];
    char addr[STR_LEN];
    bzero(name, STR_LEN);
    bzero(addr, STR_LEN);

    scanf("%s", addr);

    while(1){
        if(pos_x > 123 && pos_x < 635 && pos_y > 60 && pos_y < 125){
            pos_x = 0;
            pos_y = 0;
            printf("请输入用户名:\n");
            scanf("%s", name);
            font_pos_size_data(512, 65, 123,60, 5, 5, name);
        }
        if(pos_x > 123 && pos_x < 635 && pos_y > 162 && pos_y < 224){
            pos_x = 0;
            pos_y = 0;
            printf("请输入ip地址:\n");
            scanf("%s", addr);
            font_pos_size_data(512, 65, 123,162, 5, 5, addr);  
        }
        if(pos_x > 123 && pos_x < 635 && pos_y > 310 && pos_y < 373 && (name[0] != '\0') && (addr[0] != '\0')){
            printf("登录服务器!\n");
        }
    }
}