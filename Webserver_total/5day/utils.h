#ifndef _UTILS_H
#define _UTILS_H
#include "sort_timer_lst.h"
#include <signal.h>
class utils
{
public:
    utils() {}
    ~utils() {}


    void init(int timeslot);

    //将内核事件表注册读事件
    void addfd(int epollfd, int fd);

    //信号处理函数
    static void sig_handler(int sig);

    //设置信号函数
    void addsig(int sig, void(handler)(int), bool restart = true);

    //定时处理任务，重新定时以不断触发SIGALRM信号
    void timer_handler();

public:
    //描述管道读写端的文件描述符
    static int *u_pipefd;
    sort_timer_lst m_timer_lst;
    static int u_epollfd;
    int m_TIMESLOT;
};

void cb_func(http_parser* user_data);

#endif