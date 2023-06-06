////////////////////////////////////////////////////////////////////////
//                                                                    //
//      时间:2022-5-27                                                //
//      作者:lee                                                      //
//      内容:双向链表的结构体以及函数                                   //
//                                                                    //
//                                                                    //
////////////////////////////////////////////////////////////////////////
#include "list.h"
#include "commonheader.h"


//功  能:初始化双向链表
P_node List_Init(){

    P_node my_list = calloc(1, sizeof(Node));
    if(my_list == NULL){
        perror("双向链表初始化失败!");
        return NULL;
    }
    //使用内核链表函数进行链表初始化
    INIT_LIST_HEAD(&my_list->ptr);
    return my_list;
}

//功  能:创建新节点函数
P_node new_node(data_type data){

    P_node new = calloc(1, sizeof(Node));
    if(new == NULL){
        perror("申请新节点失败!");
        return NULL;
    }
    //数据域完成拷贝
    memcpy(&new->Data, &data, sizeof(data_type));

    //指针域自己指向自己完成初始化
    INIT_LIST_HEAD(&new->ptr);
    return new;
}

//功  能:节点插入函数
bool node_2_list(P_node my_list, P_node new, int mod){

    // if( (IS_NULL(my_list) && IS_NULL(new)) ){
    //     printf("节点异常:\n");
    //     return false;
    // }
    //判断插入模式
    switch(mod){
        case 1: list_add(&new->ptr, &my_list->ptr);         break;
        case 2: list_add_tail(&new->ptr, &my_list->ptr);    break;
        default:    printf("模式错误\n");                   break;
    }
    return true;
}

//功  能:判断节点是否为空
//返回值:指向自己(为空)--->1
//       不为空       --->0
bool IS_NULL(P_node check){
    return list_empty(&check->ptr);
}

//功  能:展示链表内容
void show_list(P_node my_list){

    // printf("这里是%s函数,第%d行\n", __FUNCTION__, __LINE__);

    if( (IS_NULL(my_list)) ){
        printf("链表异常!\n");
        return ;
    }
    P_node tmp = NULL;
    list_for_each_entry(tmp, &my_list->ptr, ptr){

        printf("该节点的类型是:%c\t名字是:%s\n", tmp->Data.type, tmp->Data.name);
    }

}

//功  能:按名字查找链表
P_node search_2_list(P_node my_list, char * name){

    if( (IS_NULL(my_list)) ){
        printf("节点异常!\n");
        return NULL;
    }
    P_node tmp = NULL;
    list_for_each_entry(tmp, &my_list->ptr, ptr){
        if( strstr(tmp->Data.name, name) != 0 ){
            printf("找到了%s这个节点\n", name);
            return tmp;
        }
    }
}



P_node get_next_node(P_node node, P_node List){

    //如果下一个节点是头节点，则直接指向头节点的下一个节点
    P_node nex = list_entry(node->ptr.next, typeof(*node), ptr);
    if(nex == List){
        nex = list_entry(List->ptr.next, typeof(*nex), ptr);
        return nex;
    }else
        return nex;
}

P_node get_prev_node(P_node node, P_node List){

    //如果上一个节点是头节点，则直接指向头节点的上一个节点
    P_node pre = list_entry(node->ptr.prev, typeof(*node), ptr);
    if(pre == List){
        pre = list_entry(List->ptr.prev, typeof(*pre), ptr);
        return pre;
    }else
        return pre;
}



//功  能: 将要显示的图片从主链表中分离到单独的链表中
P_node handle_list(P_node bmpList, P_node mainList){

    P_node tmp = NULL;
    list_for_each_entry(tmp, &mainList->ptr, ptr){
        //跳过非bmp类型的节点以及桌面图片
        if(tmp->Data.type != 'b')   continue;
        if(        (strstr(tmp->Data.name, "project"))      //跳过以下指定图片
                || (strstr(tmp->Data.name, "lock"))
                || (strstr(tmp->Data.name, "music")) 
                || (strstr(tmp->Data.name, "video")) 
                || (strstr(tmp->Data.name, "back"))
                || (strstr(tmp->Data.name, "tetris"))
                || (strstr(tmp->Data.name, "mypause"))
                || (strstr(tmp->Data.name, "mystart"))
                || (strstr(tmp->Data.name, "tetris_start"))
                || (strstr(tmp->Data.name, "confirm_reset"))
                || (strstr(tmp->Data.name, "choose_difficult"))
                || (strstr(tmp->Data.name, "good2"))
                || (strstr(tmp->Data.name, "number"))
            ){
            continue;
        }
        else {
            //方法一: 每个节点都创建新节点后插进链表（这种做法可以避免bmp链表的指针域被拷贝）
            P_node new = new_node(tmp->Data);
            if(new){
                printf("函数--->%s\t行号--->%d\n", __FUNCTION__, __LINE__);
                node_2_list(bmpList, new, 1);
            }
        }
    }
    show_list(bmpList);
    return bmpList;
}







