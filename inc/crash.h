#pragma once

#include "commonheader.h"
#include "bmp.h"
#include "touch.h"
#include "my_types.h"
#include "list.h"
#include "head4animation.h"
#include "tetris.h"

//根据方块的x, y值判断是否要与下方块发生碰撞事件
bool is_downable();

//判断 是否与右墙以及右方块发生碰撞事件
bool is_R_moveable();

//判断 是否与左墙以及左方块发生碰撞事件
bool is_L_moveable();

//处理方块与墙旋转问题
bool handle_rotate();

//判断是否可以旋转
bool is_rotate();
