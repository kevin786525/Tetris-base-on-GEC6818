#include "rank.h"

//记录分数进文件中
void record_score2file(const Record_t * usr, recordNode_t * rankList){
    FILE * fp = fopen(USR_FILE, "r+");                                          //  将文件打开权限更改为"r+"可读写状态
    if(!fp){
        perror("open file");
        return ;
    }
    Record_t tmp;
    memset(&tmp, 0, sizeof(Record_t));
    while(1){
        size_t ret = fread(&tmp, sizeof(Record_t), 1, fp);
        if(ret == 1){
            if(strcmp(usr->name, tmp.name) == 0){
                printf("找到同名的用户\n");
                int offset = sizeof(Record_t);                                  //  定义一个偏移量，找到目标之后实现光标向前偏移
                if(usr->Score > tmp.Score){                                     //  找到同名用户之后比对分数，如果更高，则覆盖原来数据
                    fseek(fp, -offset, SEEK_CUR);                               //  将光标返回到读一个数据结构体之前的位置，实现数据覆盖
                    ssize_t ret = fwrite(usr, sizeof(Record_t), 1, fp);
                    if(ret == 1){
                        printf("用户:%s\t的分数:%d已录入\n", usr->name, usr->Score);
                    }else{
                        printf("录入用户信息失败...\n");
                    }
                    fclose(fp);
                    return ;
                }else{                                                          //  如果分数比原来小或者相等，则直接返回
                    fclose(fp);
                    return ;
                }
            }
        }else{
            printf("用户信息已读取完毕!\n");
            break;
        }
    }
    fseek(fp, 0, SEEK_END);                                                     //  上面的代码实际上已经读完了文件，光标已经移至末尾，为了保险起见，将光标移至末尾
    ssize_t ret = fwrite(usr, sizeof(Record_t), 1, fp);                         //  如果上面没有找到同名用户，则直接在文件末尾录入数据
    if(ret == 1){
        printf("用户:%s\t的分数:%d已录入\n", usr->name, usr->Score);
    }else{
        printf("录入用户信息失败...\n");
    }
    fclose(fp);
}


//功  能:初始化双向链表
recordNode_t * rankList_Init(){

    recordNode_t * my_list = calloc(1, sizeof(recordNode_t));
    if(my_list == NULL){
        perror("双向链表初始化失败!");
        return NULL;
    }
    
    INIT_LIST_HEAD(&my_list->ptr);                                              //  使用内核链表函数进行链表初始化
    return my_list;
}

//创建新节点
recordNode_t * __new_node(Record_t data){
    recordNode_t * new = rankList_Init();
    if(!new){
        perror("create new node");
        return NULL;
    }
    memcpy(&new->re, &data, sizeof(Record_t));
    return new;
}


//判断是否为空链表
bool __IS_NULL(recordNode_t * check){
    return list_empty(&check->ptr);
}

//讲新节点按得分大小插入到链表中(降序)
void __sort2list(recordNode_t * rankList, recordNode_t * new){
    recordNode_t * pos;
    if(__IS_NULL(rankList)){                        //如果链表为空，则直接尾插
        list_add_tail(&new->ptr, &rankList->ptr);
    }else{
        list_for_each_entry(pos, &rankList->ptr, ptr){
            if(new->re.Score > pos->re.Score){                                          //  如果用户分数大于当前分数
                recordNode_t * pre = list_entry(pos->ptr.prev, typeof(*pos), ptr);      //  则找到当前节点的前驱节点
                __list_add(&new->ptr, &pre->ptr, &pos->ptr);                            //  利用内核链表的函数将新节点插入
                return ;
            }
        }
        list_add_tail(&new->ptr, &rankList->ptr);                                       //  如果结束的上述循环，则证明是最低分，直接尾插
    }
}


//从用户文件中读取数据
recordNode_t * read_file4record(recordNode_t ** rankList){
    FILE * fp = fopen(USR_FILE, "r");
    if(!fp){
        perror("open file");
        return NULL;
    }
    Record_t tmp;
    memset(&tmp, 0, sizeof(Record_t));
    while(1){
        ssize_t ret = fread(&tmp, sizeof(Record_t), 1, fp);
        if(ret == 1){
            // printf("读取到的用户名:%s\t分数:%d\n", tmp.name, tmp.Score);
            recordNode_t * new = __new_node(tmp);
            __sort2list(*rankList, new);        //按照分数降序插进链表
        }else{
            printf("用户信息已读取完毕!\n");
            break;
        }
    }
    fclose(fp);
}

//显示排名
void show_rankList(recordNode_t * rankList){
    if(__IS_NULL(rankList)){
        printf("list is NULL\n");
        return ;
    }
    recordNode_t * pos = rankList;
    list_for_each_entry(pos, &rankList->ptr, ptr){
        printf("[%s] 的信息: 分数-->[%d]\tip-->[%s]\t排名-->[%d]\n", 
                                                    pos->re.name, 
                                                    pos->re.Score, 
                                                    pos->re.usrAddr, 
                                                    pos->re.N_O);
    }
    return ;
}

//销毁链表链表
void destory_list(recordNode_t * rankList){
    if(__IS_NULL(rankList)){
        printf("链表为空\n");
        return ;
    }
    recordNode_t * pos = list_entry(rankList->ptr.next, typeof(*pos), ptr);
    recordNode_t * nex = list_entry(pos->ptr.next, typeof(*nex), ptr);
    for(; pos != rankList; pos = nex, nex = list_entry(pos->ptr.next, typeof(*nex), ptr)){      //通过两个指针来遍历链表，而且逐个释放节点
        free(pos);
    }
    return ;
}


//用字库显示排行榜(前三名)
void font_show_rank(recordNode_t * rankList){
    if(__IS_NULL(rankList)){
        printf("链表为空\n");
        return ;
    }
    recordNode_t * pos = list_entry(rankList->ptr.next, typeof(*pos), ptr);
    font_pos_size_data(800, 480, 0, 0, 100, 50, "用户名                分数");
    char info_buf[1024];
    memset(info_buf, 0, 64);
    for(int i = 1; i < 4 && pos != rankList; i++ ){
        snprintf(info_buf, 1024, "%s                    %d", pos->re.name, pos->re.Score);
        font_pos_size_data(800, 120, 0, 0+(i*120), 100, 50, info_buf);
        pos = list_entry(pos->ptr.next, typeof(*pos), ptr);
    }
    sleep(2);
}
