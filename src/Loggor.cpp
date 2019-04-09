#include "Loggor.h"


Logger* Logger::_instance = NULL;
abcinit init;
Logger* Logger::getInstance() {
	if (_instance == NULL)
	{
		_instance = new Logger();
	}
		
	return _instance;
}
Logger& Logger::setLinesAndLevel(const char* name, int line, int level) {
	strncpy(_TempMessage._filename, name, strlen(name));
	_TempMessage._line = line;
	_TempMessage._level = level;
	return *_instance;
}

    Logger& Logger::operator<<(const char* str) {
		pthread_mutex_lock(&_swap_lock);
	_TempMessage._time = time(NULL);//获得当前时间
	strncpy(_TempMessage._info, str, strlen(str));
	_add_message(_TempMessage);//插入缓存
	pthread_mutex_unlock(&_swap_lock);
	return *_instance;
}
Logger& Logger::operator<<(const long& str) {
	pthread_mutex_lock(&_swap_lock);
	_TempMessage._time = time(NULL);//获得当前时间
	sprintf(_TempMessage._info, " %ld ", str);
	_add_message(_TempMessage);//插入缓存
	pthread_mutex_unlock(&_swap_lock);
	return *_instance;
}
Logger& Logger::operator<<(const double& str) {
	pthread_mutex_lock(&_swap_lock);
	_TempMessage._time = time(NULL);//获得当前时间
	sprintf(_TempMessage._info, " %lf ", str);
	_add_message(_TempMessage);//插入缓存
	pthread_mutex_unlock(&_swap_lock);
	return *_instance;
}
void  Logger::_add_message(Message msg) {
	_Messagelist.push(msg);//插入缓存
	if (_Messagelist.size() == _message_max_num)
	{
		Fulsh_Messagebuff();//写入文件
	}
}

const char* level_change(int a) {
	static const char a0[] = { "DEBUG" };
	static const char a1[] = { "INFO" };
	static const char a2[] = { "\033[0;31mERROR\033[0m" };
	static const char a3[] = { "\033[0;35mWARRING\033[0m" };
	switch (a)
	{
	case 0:return a0;
	case 1:return a1;
	case 2:return a2;
	case 3:return a3;
	}
}
void  Logger::Fulsh_Messagebuff() {
	std::ofstream font(_log_file_name, std::ios::app);
	for (; _Messagelist.size();)
	{
		_TempMessage = _Messagelist.front();
		tm* a = gmtime(&_TempMessage._time);
		font << (a->tm_year + 1900) << "-" << a->tm_mon + 1 << "-" << a->tm_mday << " "
			<< a->tm_hour + 8 << ":" << a->tm_min << ":" << a->tm_sec << " "
			<< _TempMessage._filename << " Line:" << _TempMessage._line << "  "
			<< level_change(_TempMessage._level) << "  "
			<< _TempMessage._info
			<< std::endl;
		_Messagelist.pop();
	}
	font.close();
}
void  Logger::GetFilename() {
	lock_init();
	memset(_log_file_name, '\0', 50);
	mkdir("./Log", 0777);
	time_t temp = time(NULL);
	tm* a = gmtime(&temp);
	sprintf(_log_file_name, "./Log/%d_%d_%d.log", a->tm_year + 1900, a->tm_mon + 1, a->tm_mday);
	int fd = open(_log_file_name, O_RDWR | O_CREAT, 0666);
	close(fd);

}
void  Logger::lock_init() {
	pthread_mutexattr_t mutexattr;
	pthread_mutexattr_init(&mutexattr);
	pthread_mutex_init(&_swap_lock, &mutexattr);
}
void signal_demo(int)
{
	Logger::getInstance()->Fulsh_Messagebuff();
	exit(0);
}
abcinit::abcinit() { 
	Logger::getInstance()->GetFilename(); 
	signal(SIGINT, signal_demo);
}
abcinit::~abcinit() { Logger::getInstance()->Fulsh_Messagebuff(); }
