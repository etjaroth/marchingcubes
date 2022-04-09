#pragma once
#include "AbstractThreadTask.h"
#include <map>
#include <memory>
#include <thread>
#include <queue>

class ThreadManager
{
	static unsigned int timestamp;
	unsigned int numThreads;
	std::map<std::shared_ptr<AbstractThreadTask>, std::thread> activeThreads;
	std::priority_queue<std::pair<unsigned int, std::shared_ptr<AbstractThreadTask>>> pqueue;

public:
	explicit ThreadManager(unsigned int numThreads);
	~ThreadManager();

	void scheduleThread(std::shared_ptr<AbstractThreadTask> task);
	void iterate();
};