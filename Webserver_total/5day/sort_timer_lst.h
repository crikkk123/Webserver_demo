#ifndef _LIST_TIMER_H
#define _LIST_TIMER_H
#include "timer.h"
//升序双向链表（time_t expire）
class sort_timer_lst
{
public:
    sort_timer_lst();
	//链表被销毁时删除其中所有的定时器
    ~sort_timer_lst();

	//将目标定时器添加到链表中
    void add_timer(timer* timernode);

	//当某个定时任务发生变化时，调整对应的定时器在链表中的位置
	//这个函数只考虑被调整的定时器的超时时间延长的情况，
	//即该定时器需要往链表的尾端移动
    void adjust_timer(timer* timernode);
	//将目标定时器timer从链表中删除
    void del_timer(timer* timernode);
	//SIGALRM信号每次触发就在其信号处理函数（需要做信号捕捉）中执行一次tick函数，以处理链表上到期的任务
    void tick();

private:
	//一个重载的辅助函数，他被公有的add_timer函数和adjust_timer函数调用，
	//该函数表示将目标定时器timer添加到节点lst_haad之后的部分链表中
    void add_timer(timer* timernode, timer* lst_head);
private:
    timer* head;
    timer* tail;
};


#endif