#pragma once

#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#include <string>
class HttpServer{
	int _listenfd;
	int _port;
	std::string _address;
public:
	HttpServer() {}
	HttpServer(int port):_port(port){}
	int initServer();
	~HttpServer() {}
};

