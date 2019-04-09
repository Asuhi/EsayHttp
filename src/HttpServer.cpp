#include "HttpServer.h"

int HttpServer::initServer(){
	_port = 51802;
	_listenfd = -1;
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	if ((_listenfd = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
		perror("socket:");
		exit(EXIT_FAILURE);
	}
	
	//内网172.17.85.172	公网59.110.171.113
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);

	if (bind(_listenfd, (struct sockaddr*) & addr, sizeof(addr)) < 0) {
		perror("bind:");
		exit(EXIT_FAILURE);
	}
	if (listen(_listenfd, SOMAXCONN) < 0) {
		perror("listen:");
		exit(EXIT_FAILURE);
	}

	return _listenfd;
}
