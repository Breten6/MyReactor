#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <sys/syscall.h>
#include <mutex>
#include <unistd.h>
#include <thread>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>

class ThreadPool   
{
private:
	std::vector<std::thread> threads_;
	std::queue<std::function<void()>> taskqueue_; 
	std::mutex mutex_; 
	std::condition_variable condition_;
	std::atomic_bool stop_;         // set stop to true end all threads
    std::string threadtype_;
public:
	ThreadPool(size_t threadnum, const std::string& threadtype);
    void addtask(std::function<void()> task);
	size_t size();
	~ThreadPool();
};