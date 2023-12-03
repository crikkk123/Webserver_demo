#ifndef _WEB_SERVER_H
#define _WEB_SERVER_H
#include "http_parser.h"
#include "utils.h"
#define MAX_EVENT_NUMBER 2000 //监听事件的最大个数
#define MAX_FD 2000 //文件描述符上限
const int TIMESLOT = 5;
class web_server{
    public:
    web_server();
    ~web_server();
    void init(int port);
    void event_listen();
    void event_Loop(); 
    void deal_new_connection();
    void deal_read_event(int socket);
    void deal_signal_event(bool &timeout);
    void addfd(int epfd, int fd, uint32_t events);

    void newtimer(int connfd, sockaddr_in client_address);
    public:
    int m_listenfd;//监听套接字的文件描述符
    int m_port;//我们监听的端口号
    int m_epollfd;//描述epoll的文件描述符
    http_parser* users;//客户端数组
    epoll_event events[ MAX_EVENT_NUMBER ];//epoll_wait传参用

    int m_pipefd[2];
    utils util;
};



#endif