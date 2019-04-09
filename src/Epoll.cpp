#include "Epoll.h"
#include <iostream>


void Epoll::initEpoll(int lfd) {
	_lfd = lfd;
	_efd = -1;
	_efd = epoll_create1(EPOLL_CLOEXEC);
	struct epoll_event event;
	event.data.fd = lfd;
	event.events = EPOLLIN;
	epoll_ctl(_efd, EPOLL_CTL_ADD, lfd, &event);//将listenfd挂到树上
	memset(events, 0, sizeof(events));
}

void Epoll::EpollLoop(){

	int nready = epoll_wait(_efd, events, 128, -1);
	if (nready == -1) {
		if (errno == EINTR)
			return;
		else {
			perror("epoll_wait");
			exit(EXIT_FAILURE);
		}
	}
	if (nready == 0)
		return;
	for (int i = 0; i < nready; i++) {
		//如果是listenfd的话将链接请求加入到任务队列
		if (events[i].data.fd == _lfd) {//第一个是listenfd
			//Task_t task(clientConnect, (void*)this);//有客户端链接
			struct sockaddr_in peeraddr;
			socklen_t peerlen = sizeof(peeraddr);
			int conn = accept(_lfd, (struct sockaddr*) & peeraddr, &peerlen);
			struct epoll_event ev;
			ev.data.fd = conn;
			ev.events = EPOLLIN | EPOLLET;
			epoll_ctl(_lfd, EPOLL_CTL_ADD, conn, &ev);//将新链接的fd挂到树上
			std::cout << "client connected\n";
			memset(events, 0, sizeof(events));
			/*
			ThreadPool* tp = ThreadPool::getInstance();
			tp->addTask(task);//
			memset(events, 0, sizeof(events));
			*/
		}
		else if (events[i].events & EPOLLIN) {//有客户端发送数据过来
			int sock = events[i].data.fd;
			if (sock < 0)
				continue;
			//doServer((void*)sock);
			Task_t task(doServer, (void*)&sock);
			ThreadPool* tp = ThreadPool::getInstance();
			tp->addTask(task);
			memset(events, 0, sizeof(events));
		}
	}
}

void* Epoll::clientConnect(void* epo) {
	Epoll* ep = (Epoll*)epo;
	struct sockaddr_in peeraddr;
	socklen_t peerlen = sizeof(peeraddr);
	int conn = accept(ep->_lfd, (struct sockaddr*) & peeraddr, &peerlen);
	struct epoll_event ev;
	ev.data.fd = conn;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(ep->_efd, EPOLL_CTL_ADD, conn, &ev);//将新链接的fd挂到树上
	std::cout << "client connected\n";
}
void* Epoll::doServer(void* s) {
	int sock = *(int*)s;
	char buf[1024];
	int re = read(sock, buf, sizeof(buf));
	std::cout << buf << std::endl;
	const char* p = "HTTP/1.1 200 OK\r\n\r\n<h1>hello world</h1>\r\n";
	write(sock, p, strlen(p));
	close(sock);
}

