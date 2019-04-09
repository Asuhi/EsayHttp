#include "Epoll.h"
#include <iostream>


void Epoll::initEpoll(int lfd) {
	_lfd = lfd;
	_efd = -1;
	_efd = epoll_create1(EPOLL_CLOEXEC);
	struct epoll_event event;
	event.data.fd = lfd;
	event.events = EPOLLIN;
	epoll_ctl(_efd, EPOLL_CTL_ADD, lfd, &event);//��listenfd�ҵ�����
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
		//�����listenfd�Ļ�������������뵽�������
		if (events[i].data.fd == _lfd) {//��һ����listenfd
			//Task_t task(clientConnect, (void*)this);//�пͻ�������
			struct sockaddr_in peeraddr;
			socklen_t peerlen = sizeof(peeraddr);
			int conn = accept(_lfd, (struct sockaddr*) & peeraddr, &peerlen);
			struct epoll_event ev;
			ev.data.fd = conn;
			ev.events = EPOLLIN | EPOLLET;
			epoll_ctl(_lfd, EPOLL_CTL_ADD, conn, &ev);//�������ӵ�fd�ҵ�����
			std::cout << "client connected\n";
			memset(events, 0, sizeof(events));
			/*
			ThreadPool* tp = ThreadPool::getInstance();
			tp->addTask(task);//
			memset(events, 0, sizeof(events));
			*/
		}
		else if (events[i].events & EPOLLIN) {//�пͻ��˷������ݹ���
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
	epoll_ctl(ep->_efd, EPOLL_CTL_ADD, conn, &ev);//�������ӵ�fd�ҵ�����
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

