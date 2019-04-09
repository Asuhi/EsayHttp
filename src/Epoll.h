#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "ThreadPool.h"

#include <vector>
#include <queue>
class Epoll{
	int _efd;
	int _lfd;
	struct epoll_event events[128];
	int _maxEvents;
public:
	Epoll():_maxEvents(128){}
	~Epoll(){}
	void initEpoll(int lfd);
	static void* clientConnect(void* ep);
	static void* doServer(void* ep);
	void EpollLoop();
};

