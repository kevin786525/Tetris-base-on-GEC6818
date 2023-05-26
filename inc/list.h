////////////////////////////////////////////////////////////////////////
//                                                                    //
//      时间:2022-5-27                                                //
//      作者:lee                                                      //
//      内容:双向链表的结构体以及函数声明                               //
//                                                                    //
//                                                                    //
////////////////////////////////////////////////////////////////////////



#ifndef _LIST_H_
#define _LIST_H_

#include "commonheader.h"
#include "Kernel_List.h"
#include "my_types.h"

//函数名:List_Init()
//功  能:初始化双向链表
//返回值:节点指针类型
P_node List_Init();

//函数名:new_node()
//功  能:创建新节点函数
//参  数:@data自定义数据类型
//返回值:节点指针类型
P_node new_node(data_type data);

//函数名:node_2_list()
//功  能:节点插入函数
//参  数:
//          @my_list目标链表
//          @new新节点
//          @插入模式   -->1 头插法     -->2 尾插法
//返回值:bool类型
bool node_2_list(P_node my_list, P_node new, int mod);

//函数名:IS_NULL()
//功  能:判断节点是否为空
//参  数:@check判断目标
//返回值:bool类型
bool IS_NULL(P_node check);

//函数名:show_list()
//功  能:展示链表内容
//参  数:@my_list目标链表
//返回值:无
void show_list(P_node my_list);

//函数名:search_2_list()
//功  能:按名字查找链表
//参  数:@my_list目标链表   @name目标数据
//返回值:返回目标节点
P_node search_2_list(P_node my_list, char * name);



//函数名: get_next_node
//功  能: 返回当前节点的下一个节点
P_node get_next_node(P_node node, P_node List);


//函数名: get_prev_node
//功  能: 返回当前节点的上一个节点
P_node get_prev_node(P_node node, P_node List);


//功  能: 将要显示的图片从主链表中分离到单独的链表中
P_node handle_list(P_node bmpList, P_node mainList);

#endif
