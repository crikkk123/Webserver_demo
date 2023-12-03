#include "web_server.h"
web_server::web_server(){
    //分配空间
    users = new http_parser[MAX_FD];
}
web_server::~web_server(){
}

void web_server::init(int port){
    m_listenfd = -1;
    m_epollfd = -1;
    m_port = port;
    memset(events, '\0', sizeof(events));
}

void web_server::event_listen(){
    //初始化一个监听套接字
    m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
    //检错
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(m_port);
    //设置端口复用
    int flag = 1;
    setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    int ret = bind(m_listenfd, (struct sockaddr *)&address, sizeof(address));
    if(ret == -1){
        perror("bind error");
        exit(1);
    }
    listen(m_listenfd, 64);
    //创建一个epoll对象
    m_epollfd = epoll_create(64);

    http_parser::m_epollfd = m_epollfd;
    addfd(m_epollfd, m_listenfd, EPOLLIN);

    utils::u_epollfd = m_epollfd;
    pipe(m_pipefd);
    utils::u_pipefd = m_pipefd;
    addfd(m_epollfd, m_pipefd[0], EPOLLIN);
    util.init(5);

    util.addsig(SIGALRM, util.sig_handler);
    alarm(util.m_TIMESLOT);
}

void web_server::event_Loop(){
    bool timeout = false;
    while (1)
    {
        int number = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
        for (int i = 0; i < number; i++){
            int sockfd = events[i].data.fd;
            //处理新到的客户连接
            if (sockfd == m_listenfd){
                deal_new_connection();
            }
            else if(m_pipefd[0] == sockfd && events[i].events & EPOLLIN){
                deal_signal_event(timeout);
            }
            //处理客户连接上接收到的数据
            else if (events[i].events & EPOLLIN){
                deal_read_event(sockfd);
            }
        }
        if(timeout == true){
            printf("检测链表中定时器事件是否超时\n");
            util.timer_handler();
            timeout = false;
        }
    }
}

void web_server::deal_new_connection(){
    struct sockaddr_in client_address;
    socklen_t client_addrlength = sizeof(client_address);
    int connfd = accept(m_listenfd, (struct sockaddr *)&client_address, &client_addrlength);
    if (connfd < 0){
        //检错
    }
    if (http_parser::m_user_count >= MAX_FD){
        //报错
    }
    printf("新的客户端建立连接\n");
    newtimer(connfd, client_address);
}

void web_server::deal_read_event(int socket){
    users[socket].read_once();
    users[socket].do_write();
}

void web_server::addfd(int epfd, int fd, uint32_t events){
    epoll_event event;
    event.events = events;
    event.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
}

//给新的客户端分配一个定时器

 void web_server::newtimer(int connfd, sockaddr_in client_address){
    users[connfd].init(connfd, client_address);
    timer* timernode = new timer;
    timernode->user_data = &users[connfd];
    timernode->cb_func = cb_func;
    time_t cur = time(NULL);
    timernode->expire = cur + TIMESLOT * 3;
    util.m_timer_lst.add_timer(timernode);
 }

 void web_server::deal_signal_event(bool &timeout){
    int ret;
    char signal[1024];
    ret = read(m_pipefd[0], signal, sizeof(signal));
    if(ret > 0){
        timeout = true;
    }
 }