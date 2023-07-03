#pragma once

#include "Kernel_List.h"
#include "commonheader.h"
#include "font.h"

#define USR_FILE "./usr/rank.txt"

typedef struct {
    int     Score;          //记录分数
    int     N_O;            //记录排名
    char    usrAddr[50];    //记录该用户的IP地址
    char    name[256];      //记录用户名
}Record_t;


typedef struct {
    Record_t            re;
    struct list_head    ptr;
}recordNode_t;


//记录分数
void record_score2file(const Record_t * usr, recordNode_t * rankList);


//用户名登录游戏
void sign_in(Record_t * usr);

//功  能:初始化双向链表
recordNode_t * rankList_Init();


//从用户文件中读取数据
recordNode_t * read_file4record(recordNode_t ** rankList);

//显示排名
void show_rankList(recordNode_t * rankList);

//删除链表
void destory_list(recordNode_t * rankList);


//用字库显示排行榜(前三名)
void font_show_rank(recordNode_t * rankList);