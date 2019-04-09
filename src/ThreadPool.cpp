#include "ThreadPool.h"
#include <iostream>
#include <pthread.h>

ThreadPool* ThreadPool::_instance = NULL;

void ThreadPool::routine(ThreadPool* tp) {
	pthread_detach(pthread_self());//Ïß³Ì·ÖÀë
	while (true) {
		tp->_mtx.lock();
		while (tp->_task.empty()) {
			tp->_cond.wait(tp->_mtx);
		}
		Task_t tk = tp->_task.front();
		tp->_task.pop();
		tp->_mtx.unlock();
		tk.run();
	}
}

void ThreadPool::initThreadPool() {
	for (int i = 0; i < _num; ++i) {
		std::thread td(routine,this);
		td.detach();
	}
}


void ThreadPool::addTask(const Task_t& task) {
	_mtx.lock();
	_task.push(task);
	_cond.notify_one();
	_mtx.unlock();
}
