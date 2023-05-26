/*
 * Copyright (C) 2007 Oracle.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License v2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 021110-1307, USA.
 */

#ifndef _LINUX_LIST_H
#define _LINUX_LIST_H

#define LIST_POISON1  ((struct list_head *) 0x00100100)
#define LIST_POISON2  ((struct list_head *) 0x00200200) //用于区分空指针是否为认为指向空地址，用于程序调试


/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

/**
 * 在大结构体中定义一个小型结构体作为指针域 
 */
struct list_head {
	struct list_head *next, *prev;
};

/**
 * 宏定义LIST_HEAD_INIT(name)，将name结构体
 * （内核链表节点中的指针域）中的*next, *prev取
 * 地址村放入数组中.
 */
#define LIST_HEAD_INIT(name) { &(name), &(name) }



/**
 * 宏定义LIST_HEAD(name)，
 * 使用时创建一个name结构体（内核链表节点中的指针域），
 * 并直接调用宏LIST_HEAD_INIT(name)
 */
#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

/**
 * 节点初始化
 * 
 * inline用于修饰函数INIT_LIST_HEAD(struct list_head *list)为内联函数，
 * 因此必须用static修饰将函数固定在本文件，如此可实现内联函数用空间换取效率的功能
 * 函数INIT_LIST_HEAD(struct list_head *list)用于将内核链表节点的指针域中的指针指向自己本身，
 * 起到节点初始化的作用
 */
static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}


/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
#ifndef CONFIG_DEBUG_LIST
/**
 * 节点插入
 * 
 * 当定义了CONFIG_DEBUG_LIST后，
 * 将使用内联函数__list_add(struct list_head *xnew,struct list_head *prev,struct list_head *next)
 * 函数__list_add的作用是：将 xnew 插入到目标 prev 和目标  next 之间，
 * 是内核链表的节点插入函数，可同时实现头插和尾插
 */
static inline void __list_add(struct list_head *xnew,
			      struct list_head *prev,
			      struct list_head *next)
{
						next->prev = xnew;
						xnew->next = next;
						xnew->prev = prev;
						prev->next = xnew;
}
#else
/**
 * 若未定义CONFIG_DEBUG_LIST
 * 则以函数调用的形式使用__list_add
 */
extern void __list_add(struct list_head *xnew,
			      struct list_head *prev,
			      struct list_head *next);
#endif


/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
#ifndef CONFIG_DEBUG_LIST
/**
 * 节点头插
 * 
 * 使用内联函数list_add(struct list_head *xnew, struct list_head *head)，
 * 将会自动调用__list_add，实现功能：将节点 xnew 插入到 head 后面；实现头插功能 
 */
static inline void list_add(struct list_head *xnew, struct list_head *head)
{
							__list_add(xnew, head, head->next);
}
#else
/**
 * 若未定义CONFIG_DEBUG_LIST则以函数调用的形式使用list_add；
 */
extern void list_add(struct list_head *xnew, struct list_head *head);
#endif																



/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */

/**
 * 节点尾插
 * 
 * 使用内联函数list_add_tail(struct list_head *xnew, struct list_head *head)，
 * 将会自动调用__list_add，实现功能：将节点 xnew 插入到 head 前面；实现尾插功能 
 */
static inline void list_add_tail(struct list_head *xnew, struct list_head *head)
{
	__list_add(xnew, head->prev, head);
}


/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */

/**
 * 节点删除
 * 
 * 使用内联函数__list_del(struct list_head * prev, struct list_head * next)，
 * 实现功能：将节点 prev 和节点 next 之间的节点移除出链表
 */
static inline void __list_del(struct list_head * prev, struct list_head * next)
{
							next->prev = prev;
							prev->next = next;
}

/***********************  ***********************/
/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty on entry does not return true after this, the entry is
 * in an undefined state.
 */
/**
 * 节点删除并指向空
 * 
 * 使用内联函数list_del(struct list_head *entry)，
 * 将会自动调用__list_del，实现功能：将节点 entry 移除出链表
 * 过程如下：
 * 1 调用函数__list_del将 entry 的前驱结点和后继节点传递到函数__list_del
 * 2 将 entry 的后继指针人为指向 LIST_POISON1 ，即让指针人为指向空， 解引用会报页错误
 * 3 将 entry 的前驱指针人为指向 LIST_POISON1 ，即让指针人为指向空， 解引用会报页错误
 */
#ifndef CONFIG_DEBUG_LIST
static inline void list_del(struct list_head *entry)				
{ 
	__list_del(entry->prev, entry->next);
	entry->next = LIST_POISON1; // 0x00100100
	entry->prev = LIST_POISON2; // 0x00200200
}
#else
/**
 * 若未定义CONFIG_DEBUG_LIST则以函数调用的形式使用list_del；
 */
extern void list_del(struct list_head *entry);
#endif



/**
 * list_replace - replace old entry by new one
 * @old: the element to be replaced
 * @new: the new element to insert
 * Note: if 'old' was empty, it will be overwritten.
 */
/**
 * 新旧节点替换
 * 
 * 使用内联函数list_replace(struct list_head *old,struct list_head *xnew)，
 * 实现功能：将节点 xnew 替换节点 old 节点，将old移除出链表
 * 过程如下：
 * 1 节点 xnew 的后继指针指向节点 old 的后继节点
 * 2 节点 xnew 的后继节点的前驱指针指向节点 xnew ，此时节点 old 与本身的后继节点彻底断开
 * 3 节点 xnew 的前驱指针指向节点 old 的前驱节点
 * 4 节点 xnew 的前驱节点的后继指针指向节点 xnew ，此时节点 old 与本身的前驱节点彻底断开  节点 old 移除成功
 */
static inline void list_replace(struct list_head *old,
				struct list_head *xnew)
{ 
	xnew->next = old->next; 
	xnew->next->prev = xnew;
	xnew->prev = old->prev;
	xnew->prev->next = xnew;
}



/**
 * 新旧节点替换 - 旧节点指向空
 * 
 * 使用内联函数list_replace_init(struct list_head *old,struct list_head *xnew)，
 * 实现功能：将节点 xnew 替换节点 old 节点，将old移除出链表，并将节点 old 初始化
 * 
 * 使用内联函数list_replace将节点 xnew 替换节点 old 节点，将old移除出链表
 * 使用函数INIT_LIST_HEAD将节点 old 的前驱指针和后继指针同时指向自己进行初始化，防止出现野指针
 */ 
static inline void list_replace_init(struct list_head *old,			
					struct list_head *xnew)
{
	list_replace(old, xnew);
	INIT_LIST_HEAD(old);
}



/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
/**
 * 节点删除并初始化
 * 
 * 使用内联函数list_del_init(struct list_head *entry)，
 * 实现功能：将节点 entry 移除出链表并初始化节点 entry
 * 
 * 调用函数__list_del将 entry 的前驱结点和后继节点传递到函数__list_del并移除节点 entry
 * 使用函数INIT_LIST_HEAD将节点 entry 的前驱指针和后继指针同时指向自己进行初始化，防止出现野指针
 * */
static inline void list_del_init(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	INIT_LIST_HEAD(entry);
}



/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
/**
 * 节点头插移位 
 * 
 * 使用内联函数list_move(struct list_head *list, struct list_head *head)，
 * 实现功能：将链表中节点 list 移动到节点 head（头节点） 后面（实现链表内节点前置）
 * 
 * 主要用于将链表的任意非头节点前置到链表头部，也可实现链表任意节点以头插形式进行移位
 * 调用函数__list_del将节点 list 的前驱结点和后继节点传递到函数__list_del并移除节点 list 并且无需初始化
 * 调用函数list_add将节点 list 插入到节点 head（头节点） 后面，
 * 实现链表中已存在的节点进行位移（将链表中节点插入链表头部）
 * */
static inline void list_move(struct list_head *list, struct list_head *head)
{
        __list_del(list->prev, list->next);
        list_add(list, head);
}


/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
/**
 * 节点尾插移位
 * 
 * 使用内联函数list_move_tail(struct list_head *list, struct list_head *head)，
 * 实现功能：将链表中节点 list 移动到节点 head（头节点） 前面（实现链表内节点后置）
 * 
 * 主要用于将链表的任意非头节点后置到链表尾部，也可实现链表任意节点以尾插形式进行移位
 * 调用函数__list_del将节点 list 的前驱结点和后继节点传递到函数__list_del并移除节点 list 并且无需初始化
 * 调用函数list_add_tail将节点 list 插入到节点 head（头节点）前面，
 * 实现链表中已存在的节点进行位移（将链表中节点插入链表尾部）
 */
static inline void list_move_tail(struct list_head *list,
				  struct list_head *head)
{
        __list_del(list->prev, list->next);
        list_add_tail(list, head);
}



/**
 * list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
/**
 * 判断尾部节点
 * 
 * 使用内联函数list_is_last(const struct list_head *list,const struct list_head *head)，
 * 实现功能：检测链表中节点 list 是否为链表尾部节点
 * 
 * 函数定义为整形，由于非零即真，当被检测节点 list 为尾部节点则返回 true（1） ，否则返回 false（0）
 */
static inline int list_is_last(const struct list_head *list,
				const struct list_head *head)
{
	return list->next == head;
}



/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
/**
 * 空链表判断
 * 
 * 使用内联函数list_empty(const struct list_head *head)，
 * 实现功能：检测链表中头节点 head 的后继节点是否为本身，达到检测链表是否为空的目的
 * 
 * 函数定义为整形，由于非零即真，当被检测节点 head 的后继节点为头节点本身则返回 true（1）即链表为空，
 * 否则返回 false（0）
 */
static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}


/**
 * list_empty_careful - tests whether a list is empty and not being modified
 * @head: the list to test
 *
 * Description:
 * tests whether a list is empty _and_ checks that no other CPU might be
 * in the process of modifying either member (next or prev)
 *
 * NOTE: using list_empty_careful() without synchronization
 * can only be safe if the only activity that can happen
 * to the list entry is list_del_init(). Eg. it cannot be used
 * if another CPU could re-list_add() it.
 */
/**
 * 防止多线程同时操作一个链表 - 兼顾空链表判断
 * 使用内联函数list_empty_careful(const struct list_head *head)，
 * 实现功能：检测链表中头节点 head 的后继节点以及前驱节点是否为本身，达到检测链表是否为空的目的
 * 
 * 定义一个全新节点 next 作为头节点 head 的后继节点
 * 函数定义为整形，由于非零即真，当节点 next 为头节点 head 本身，
 * 且节点 next 为头节点 head 的前驱节点则返回 true（1）即链表为空，否则返回 false（0）
 * 此函数是函数list_empty的升级版，从两方向来证明链表为空链表。
 */
static inline int list_empty_careful(const struct list_head *head)
{
	struct list_head *next = head->next;
	return (next == head) && (next == head->prev);
}
/*
 * 尝试理解：
 * 当不同步使用函数list_empty_careful时，链表唯一能安全操作的函数是list_del_init。
 * 即另一个CPU屋发对该链表进行插入节点操作（即不能被别的CUP调用函数list_add进行链表节点更改）。
 * 即防止多个CPU同时对这个链表进行修改。
 */


/**
 * 链表合并（首尾相接）通用部分
 * 
 * 为了方便理解这个函数的作用，可以按下面的参数列表进行理解：
 * 函数形参中：
 * list -- 需要被合并的链表的头节点
 * prev -- 另一个链表的头节点
 * next -- 头节点为 prev 的后继节点
 * 函数内部：
 * last -- list的前驱节点，即list链表的尾部节点
 * first -- list的后继节点，即list链表（除头节点外）的第一个节点
 * 
 * 使用内联函数 __list_splice(const struct list_head *list,struct list_head *prev,struct list_head *next)，
 * 实现功能：将list链表与另一个链表合并
 * 过程如下：
 * 1 first -- list的后继节点，即list链表（除头节点外）的第一个节点
 * 2 last -- list的前驱节点，即list链表的尾部节点
 * 3 节点 first 的前驱指针指向节点 prev ，即list链表第一个节点前驱指针指向合并对象的头节点
 * 4 节点 prev 的后继指针指向节点 first ，即合并对象的头节点后继指针指向list链表的第一个节点
 * 5 节点 last 的后继指针指向节点 next ，即list链表的尾部节点后继指针指向合并对象链表（非头结点）第一个节点
 * 6 节点 next 的前驱指针指向节点 last ，即合并对象链表（非头结点）第一个节点前驱指针指向list链表的尾部指针
 * 7 到此时，节点 prev 替代了list链表的头几点，合并效果为list链表除头节点 list 外，
 *   其余节点按原顺序插入到合并对象链表（即原prev链表）的头节点 prev 和原链表第一个非头节点之间
 */
static inline void __list_splice(const struct list_head *list,
				 struct list_head *prev,
				 struct list_head *next)
{
	struct list_head *first = list->next;
	struct list_head *last = list->prev;

	first->prev = prev;
	prev->next = first;

	/*
	 * 注意:
	 * 在本次理解中，由于节点 next 
	 * 以合并对象头节点 prev 的后继节点的身份
	 * （即函数形参中 next == prev->next）传入此函数，
	 * 即便头节点 prev 后继指针对象改变，
	 * 节点 next 仍为原合并对象链表的第一个非头节点，
	 * 而非新的 prev->next。
	*/

	last->next = next;
	next->prev = last;
}
/**
 * 本次理解的效果例子如下：
 * 			list链表			prev链表			合并后链表
 * 头节点     list               prev                 prev
 * 				0				   4					0
 * 				1				   5					1
 * 				2				   6					2
 * 														4
 * 														5
 * 														6
 * 遗留问题：上述过程中，个人并没有发现节点 list 的前驱与后继指针被初始化的操作，因此即便合并成功，在本次理解中节点 list 前驱指针仍指向合并后链表的节点 2 ，后继指针仍指向合并后链表节点 0 ,这样岂不是很不安全？
 * 温馨提示：由于传入函数的参数可随程序设计而改变，因此可实现多种效果， 以上为其中之一。
*/



/**
 * list_splice - join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
/**
 * 链表前置合并前面
 * 
 * 使用内联函数 list_splice(struct list_head *list, struct list_head *head)，
 * 实现功能：将list链表与head链表合并
 * 
 * 调用list_empty函数判断list链表是否为空
 * 调用__list_splice函数将链表list首尾相接到head链表头部，并用节点 head 作为合成链表的头节点
 */
static inline void list_splice(struct list_head *list, struct list_head *head)
{
	if (!list_empty(list))
		__list_splice(list, head, head->next);
}


/**
 * list_splice_tail - join two lists, each list being a queue
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
/**
 * 链表后置合并
 * 
 * 使用内联函数 list_splice_tail(struct list_head *list, struct list_head *head)，
 * 实现功能：将list链表与head链表合并
 * 
 * 调用list_empty函数判断list链表是否为空
 * 调用__list_splice函数将链表list首尾相接到head链表尾部，并用节点 head 作为合成链表的头节点
 */
static inline void list_splice_tail(struct list_head *list,
				struct list_head *head)
{
	if (!list_empty(list))
		__list_splice(list, head->prev, head);
}



/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
/**
 * 链表前置合并并初始化被合并链表的头节点
 * 
 * 使用内联函数 list_splice_init(struct list_head *list, struct list_head *head)，
 * 实现功能：将list链表与head链表合并
 * 
 * 调用list_empty函数判断list链表是否为空
 * 调用__list_splice函数将链表list首尾相接到head链表头部，并用节点 head 作为合成链表的头节点
 * 调用INIT_LIST_HEAD函数初始化节点 list （合并通用部分提到的问题解决方案），防止节点仍与合并后链表有链接，影响链表后续操作
 */
static inline void list_splice_init(struct list_head *list,
				    struct list_head *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head, head->next);
		INIT_LIST_HEAD(list);
	}
}


/***********************  ***********************/
/**
 * list_splice_tail_init - join two lists and reinitialise the emptied list
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */
/**
 * 链表后置合并并初始化被合并链表的头节点
 * 
 * 使用内联函数 list_splice_tail_init(struct list_head *list, struct list_head *head)，
 * 实现功能：将list链表与head链表合并
 * 
 * 调用list_empty函数判断list链表是否为空
 * 调用__list_splice函数将链表list首尾相接到head链表尾部，并用节点 head 作为合成链表的头节点
 * 调用INIT_LIST_HEAD函数初始化节点 list （合并通用部分提到的问题解决方案），防止节点仍与合并后链表有链接，影响链表后续操作
 */
static inline void list_splice_tail_init(struct list_head *list,
					 struct list_head *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head->prev, head);
		INIT_LIST_HEAD(list);
	}
}



/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
/**
 * 寻找内核链表节点的入口地址
 * 
 * ptr    为头节点指针域
 * type   嵌入节点结构体的类型
 * member 节点结构体中成员的名称
 * 
 * 计算内核链表节点入口地址与指针域结构体的地址差值
 */
#define list_entry(ptr, type, member) \
	((type*)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))



/**
 * list_first_entry - get the first element from a list							
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
/**
 * 获取链表第一个或者指定节点的下一个节点的入口地址
 * 
 * 调用函数list_entry，获取节点指针域结构体 ptr 的后继节点入口地址
 */
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)



/**
 * list_next_entry - get the next element from a list
 * @ptr:	the list head to take the element from.
 * @member:	the name of the list_struct within the struct.
 *
 * Note, that next is expected to be not null.
 */
/**
 * 遍历给定类型的链表
 * 
 */
#define list_next_entry(ptr, member) \
	list_entry((ptr)->member.next, typeof(*ptr), member)


/* ======================================================================================================================================================= */
//此处分界线目的是区分小结构体（指针域）的函数以及大结构体（节点）的函数



/**
 * 通过指针域遍历所有小结构体（指针域）
 * 
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); \
        	pos = pos->next)
/*
 * 这里更新一下，list_for_each和__list_for_each是有不同的
 * 原型是这样:
#define list_for_each(pos, head) \
	for (pos = (head)->next; prefetch(pos->next), pos != (head); \
        	pos = pos->next)
 * prefetch函数是预读取功能函数，C语言中比较少见，主要出现在前端设计中
 * 拓展：
 * 基于VUE的前端渲染时，HTML页面的打开会用到资源预读取函数，与prefetch相对应的函数是preload；
 * 函数preload无论在什么情况下都会直接预读取资源，浏览器就会加载相应的资源；
 * 函数prefetch则是会告诉浏览器可能需要某些资源，但不一定会加载；
 * 把这个解释放在这里有点牵强，但是可以尝试去理解
*/

/**
 * 用于短链表或空链表的遍历
 * 
 * __list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 *
 * This variant differs from list_for_each() in that it's the
 * simplest possible list iteration code, no prefetching is done.
 * Use this for code that knows the list to be very short (empty
 * or 1 entry) most of the time.
 */
#define __list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)
/**
 * 相较于函数list_for_each的原型，此函数没有进行预取。
 * 很遗憾我没有看出差别来 -_-
 */



/**
 * 反向遍历整个链表
 * 
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); \
        	pos = pos->prev)



/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
/**
 * 正向链表遍历且删除小结构体（指针域）
 * 
 * 相较于函数list_for_each，此函数设置了双重保险，用于检测链表遍历是否到头
 * 值得关注的是，当链表读到的节点被删除时，pos = pos->next是无法使用的，因此需要用第二重保险 n 来解决问题，故删除节点遍历是需要此函数
 */
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * 为此我尝试写一下反向遍历的安全版
 * 反向链表遍历且删除小结构体（指针域）
 #define list_for_each_prev(pos, head) \
   for (pos = (head)->prev; n = pos->next; pos != (head); \
        pos = n, n = pos->prev)
 * 嗯~，不知道能不能成，测试就交给你们了哈哈哈 o(*￣︶￣*)o ，当然也可以尝试使用预处理命令prefetch，不过目前用处不大
 */

/* ======================================================================================================================================================= */
//接下来的函数将会是上面函数的升级，主要用于内核链表节点这种大结构体



/**
 * 正向遍历给定类型链表的同时寻找各链表入口地址
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.			//大结构体的指针
 * @head:	the head for your list.						//小结构体的头指针（头节点）
 * @member:	the name of the list_struct within the struct.	//大结构体里面需要找到的哪位成员
 */
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))// 效果相当于在函数list_for_each基础上，加上函数



/**
 * 反向遍历链表的同时寻找各链表入口地址
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry_reverse(pos, head, member)			\
	for (pos = list_entry((head)->prev, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = list_entry(pos->member.prev, typeof(*pos), member))



/**
 * 个人猜测：遍历内核链表的标记节点的初始化（准备）
 * list_prepare_entry - prepare a pos entry for use in list_for_each_entry_continue
 * @pos:	the type * to use as a start point
 * @head:	the head of the list
 * @member:	the name of the list_struct within the struct.
 *
 * Prepares a pos entry for use as a start point in list_for_each_entry_continue.
 */
#define list_prepare_entry(pos, head, member) \
	((pos) ? : list_entry(head, typeof(*pos), member))



/**
 * 给定节点类型的链表的正向遍历
 * list_for_each_entry_continue - continue iteration over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define list_for_each_entry_continue(pos, head, member) 		\
	for (pos = list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head);	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))



/**
 * 当前节点的正向遍历
 * list_for_each_entry_from - iterate over list of given type from the current point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define list_for_each_entry_from(pos, head, member) 			\
	for (; &pos->member != (head);	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))


/**
 * 正向链表遍历且删除大结构体
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_entry((head)->next, typeof(*pos), member),	\
		n = list_entry(pos->member.next, typeof(*pos), member);	\
	    &pos->member != (head); 					\
		pos = list_entry(n->member.next, typeof(*n), member),	\
		n 	= list_entry(pos->member.next, typeof(*pos), member))
	    //pos = n, n = list_entry(n->member.next, typeof(*n), member))// 具体可参考list_for_each_safe函数

		
//仿照内核链表实现一个可以交替访问双节点的循环
// #define list_for_ech_entry_safe_alternate(pos, n, head, member)	\
// 	for (pos = list_entry((head)->next, typeof(*pos), member),	\
// 		n = list_entry(pos->member.next, typeof(*pos), member);	\
// 		&pos->member != (head);									\
// 		pos = list_entry(n->member.next, typeof(*n), member),	\
// 		n 	= list_entry(pos->member.next, typeof(*pos), member))	//主要改动是将每次循环是将pos指向n的下一个节点，n指向当前n的下下个节点

/**
 * 给定节点类型的链表的正向遍历且删除大结构体
 * list_for_each_entry_safe_continue
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define list_for_each_entry_safe_continue(pos, n, head, member) 		\
	for (pos = list_entry(pos->member.next, typeof(*pos), member), 		\
		n = list_entry(pos->member.next, typeof(*pos), member);		\
	     &pos->member != (head);						\
	     pos = n, n = list_entry(n->member.next, typeof(*n), member))// 删除节点遍历是必须使用此函数，参考list_for_each_safe



/**
 * 给定节点类型的链表的正向遍历且删除大结构体
 * list_for_each_entry_safe_from
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define list_for_each_entry_safe_from(pos, n, head, member) 			\
	for (n = list_entry(pos->member.next, typeof(*pos), member);		\
	     &pos->member != (head);						\
	     pos = n, n = list_entry(n->member.next, typeof(*n), member))// 删除节点遍历是必须使用此函数，参考list_for_each_safe



/**
 * 反向链表遍历且删除大结构体（指针域）
 * list_for_each_entry_safe_reverse
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define list_for_each_entry_safe_reverse(pos, n, head, member)		\
	for (pos = list_entry((head)->prev, typeof(*pos), member),	\
		n = list_entry(pos->member.prev, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = list_entry(n->member.prev, typeof(*n), member))

#endif
