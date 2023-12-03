#include "sort_timer_lst.h"

sort_timer_lst::sort_timer_lst(){
    head = NULL;
    tail = NULL;
}
sort_timer_lst::~sort_timer_lst(){
    timer* tmp = head;
    while(tmp){
        head = tmp->next;
        delete tmp;
        tmp = head;
    }
}

void sort_timer_lst::add_timer(timer* timernode){
    if (!timernode)
    {
        return;
    }
    if (!head)
    {
        head = tail = timernode;
        return;
    }
    if (timernode->expire < head->expire)
    {
        timernode->next = head;
        head->prev = timernode;
        head = timernode;
        return;
    }
    add_timer(timernode, head);
}

void sort_timer_lst::add_timer(timer *timernode, timer *lst_head)
{
    timer *prev = lst_head;
    timer *tmp = prev->next;
    while (tmp)
    {
        if (timernode->expire < tmp->expire)
        {
            prev->next = timernode;
            timernode->next = tmp;
            tmp->prev = timernode;
            timernode->prev = prev;
            break;
        }
        prev = tmp;
        tmp = tmp->next;
    }
    if (!tmp)
    {
        prev->next = timernode;
        timernode->prev = prev;
        timernode->next = NULL;
        tail = timernode;
    }
}
//有序链表的某个节点更新之后需要重新排序
void sort_timer_lst::adjust_timer(timer *timernode)
{
    if (!timernode)
    {
        return;
    }
    timer *tmp = timernode->next;
    if (!tmp || (timernode->expire < tmp->expire))
    {
        return;
    }
    if (timernode == head)
    {
        head = head->next;
        head->prev = NULL;
        timernode->next = NULL;
        add_timer(timernode, head);
    }
    else
    {
        timernode->prev->next = timernode->next;
        timernode->next->prev = timernode->prev;
        add_timer(timernode, timernode->next);
    }
}

void sort_timer_lst::del_timer(timer *timernode)
{
    if (!timernode)
    {
        return;
    }
    if ((timernode == head) && (timernode == tail))
    {
        delete timernode;
        head = NULL;
        tail = NULL;
        return;
    }
    if (timernode == head)
    {
        head = head->next;
        head->prev = NULL;
        delete timernode;
        return;
    }
    if (timernode == tail)
    {
        tail = tail->prev;
        tail->next = NULL;
        delete timernode;
        return;
    }
    timernode->prev->next = timernode->next;
    timernode->next->prev = timernode->prev;
    delete timernode;
}

void sort_timer_lst::tick()
{
    if (!head)
    {
        return;
    }
    time_t cur = time(NULL);
    timer *tmp = head;
    while (tmp)
    {
        if (cur < tmp->expire)//数小的超时
        {
            break;
        }
        tmp->cb_func(tmp->user_data);
        head = tmp->next;
        if (head)
        {
            head->prev = NULL;
        }
        delete tmp;
        tmp = head;
    }
}