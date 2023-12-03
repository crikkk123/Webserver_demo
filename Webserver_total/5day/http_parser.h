#ifndef _HTTP_PARSER_H
#define _HTTP_PARSER_H
#include <stdio.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <sys/epoll.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "timer.h"
class timer;
class http_parser{
    public:
    static const int BUFFER_SIZE = 1024;
    public:
    void init(int socket, sockaddr_in cliaddr);
    void read_once();
    void do_write();
    void addfd(int epfd, int fd, uint32_t events);

    public:
    int m_socket;
    sockaddr_in address;
    char m_buf[BUFFER_SIZE];
    int m_buf_len;
    static int m_epollfd;
    static int m_user_count;

    timer* m_timer;
};
#endif