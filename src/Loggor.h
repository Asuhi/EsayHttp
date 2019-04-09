#pragma once

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <queue>
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <stdio.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

struct  Message {
	char _filename[50];
	int  _line;
	int _level;
	char _info[1024];
	time_t _time;
};
class abcinit {
public:
	abcinit();
	~abcinit();
};
class Logger {
private:
	static Logger* _instance;
	Logger() {
		_message_max_num = 32;//最大缓存数
	}

	Message _TempMessage; //单个信息缓存
	std::queue<Message> _Messagelist;//信息缓存表
	void _add_message(Message msg);//添加信息到缓存表
	int _message_max_num;//最大缓存数
	char _log_file_name[50];
	pthread_mutex_t _swap_lock;//交换区锁
	void lock_init();
public:
	
	void GetFilename();
	void Fulsh_Messagebuff();
	static Logger* getInstance();
	Logger& setLinesAndLevel(const char* name, int line, int level);
	Logger& operator<<(const char* str);
	Logger& operator<<(const long& str);
	Logger& operator<<(const double& str);
};

#define LOG_DEBUG  ((*Logger::getInstance()).setLinesAndLevel(__FILE__,__LINE__,0))
#define LOG_INFO   ((*Logger::getInstance()).setLinesAndLevel(__FILE__,__LINE__,1))
#define LOG_ERROR  ((*Logger::getInstance()).setLinesAndLevel(__FILE__,__LINE__,2))
#define LOG_WORING ((*Logger::getInstance()).setLinesAndLevel(__FILE__,__LINE__,3))



