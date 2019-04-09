#include <cstdio>
#include <thread>
#include <iostream>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include "ThreadPool.h"
#include "Epoll.h"
#include "HttpServer.h"
#include <thread>
#include <condition_variable>
#include <atomic>
#include "Loggor.h"

int main() {
    HttpServer server;
    Epoll ep;
	ep.initEpoll(server.initServer());
    LOG_DEBUG << "init over";
    std::cout<<"ok,"<<std::endl;
    while (1) {
		ep.EpollLoop();
	}
	return 0;
}


