#pragma once
#include <atomic>
#include <vector>
#include <thread>
#include <queue>
#include <condition_variable>
#include <boost/noncopyable.hpp>

typedef void* (*func_t)(void*);

class Task_t {
private:
	void* _arg ;
	func_t _func;
public:
	Task_t(func_t func,void* arg):_func(func),_arg(arg){}
	void run() {
		_func(_arg);
	}
	~Task_t() {}
};

class ThreadPool final: boost::noncopyable{
private:
	std::mutex _mtx;
	std::queue<Task_t> _task;
	int _num;
	std::condition_variable_any _cond;
	static ThreadPool* _instance;
	ThreadPool(int num) :_num(num) { initThreadPool(); }
	void initThreadPool();
public:
	static ThreadPool* getInstance() {
		if (_instance == NULL) {
			_instance = new ThreadPool(4);
		}
		return _instance;
	}
	
	static void routine(ThreadPool* tp);
	void addTask(const Task_t& task);
	~ThreadPool(){}
};
