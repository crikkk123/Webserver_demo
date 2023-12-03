#include "http_parser.h"

int http_parser::m_epollfd = -1;
int http_parser::m_user_count = 0;

void http_parser::init(int socket, sockaddr_in cliaddr){
    m_socket = socket;
    address = cliaddr;
    memset(m_buf, '\0', BUFFER_SIZE);
    m_buf_len = 0;
    addfd(m_epollfd, m_socket, EPOLLIN);
}

void http_parser::addfd(int epfd, int fd, uint32_t events){
    epoll_event event;
    event.events = events;
    event.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
}

void http_parser::read_once(){
    m_buf_len = read(m_socket, m_buf, BUFFER_SIZE);
}
void http_parser::do_write(){
    write(1, m_buf, m_buf_len);
    write(m_socket, m_buf, m_buf_len);
}
