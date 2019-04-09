#include "Epoll.h"
#include <iostream>
#include "Loggor.h"

void Epoll::initEpoll(int lfd) {
	_lfd = lfd;
	_efd = -1;
	_efd = epoll_create1(EPOLL_CLOEXEC);
	struct epoll_event event;
	event.data.fd = lfd;
	event.events = EPOLLIN | EPOLLET;
	epoll_ctl(_efd, EPOLL_CTL_ADD, lfd, &event);//将listenfd挂到树上
}

void Epoll::EpollLoop(){

	int nready = epoll_wait(_efd, events, 128, -1);
	if (nready == -1) {
			perror("epoll_wait");
			exit(EXIT_FAILURE);

	}
	if (nready == 0)
		return;
	for (int i = 0; i < nready; i++) {
		//如果是listenfd的话将链接请求加入到任务队列
		if (events[i].data.fd == _lfd) {//第一个是listenfd
            
            Task_t task(clientConnect, (void*)this);//有客户端链接
			ThreadPool* tp = ThreadPool::getInstance();
			tp->addTask(task);//
		}
		else if (events[i].events & EPOLLIN){
			int sock = events[i].data.fd;
			if (sock < 0)
				continue;
            int fd[2] = {_efd ,sock};

            Task_t task(doServer, (void*)fd);
			ThreadPool* tp = ThreadPool::getInstance();
			tp->addTask(task);
	    }
    }
}

void* Epoll::clientConnect(void* epo) {
	Epoll* ep = (Epoll*)epo;
	struct sockaddr_in peeraddr;
	socklen_t peerlen = sizeof(peeraddr);
	int conn = accept(ep->_lfd, (struct sockaddr*) & peeraddr, &peerlen);
    struct epoll_event ev;
    int flag = fcntl(conn,F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(conn,F_SETFL,flag);
    ev.data.fd = conn;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(ep->_efd, EPOLL_CTL_ADD, conn, &ev);//将新链接的fd挂到树上
    return NULL;
}
void* Epoll::doServer(void* epo) {
    int* sock = (int*)epo;
    int epfd = *sock;
    sock++;
    int conn = *sock;
    char buf[1024];
	int ret = read(conn, buf, sizeof(buf));
    if(ret == 0){
        close(conn);
        return NULL;
    }
    int fd = open("index.html",O_RDONLY);
    const char* p = "HTTP/1.1 200 OK\r\nContent-Length: 2245\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n";
    write(sock,p,strlen(p));
    while(true){
        int ret = read(fd,buf,sizeof(buf));
        
        if(ret == 0)
            break;
        write(sock,buf,ret);
    }
	LOG_DEBUG << "Response over";
    //close(sock);
    return NULL;
}

