#ifndef _TIMER_H
#define _TIMER_H
#include <time.h>
#include "http_parser.h"
class http_parser;
class timer
{
    public:
        timer() : prev( NULL ), next( NULL ){}
    public:
    time_t expire; //任务的超时时间，这里使用绝对时间
    void (*cb_func)( http_parser* );//任务回调函数
    //回调函数处理的客户数据，由定时器的执行者传递给回调函数
    http_parser* user_data;
    timer* prev;//指向前一个定时器
    timer* next;//指向下一个定时器
};

#endif