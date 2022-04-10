#pragma once
#include "AbstractThreadTask.h"
#include "ThreadHolder.h"
#include <vector>

class ThreadManager
{
	std::vector<std::shared_ptr<ThreadHolder>> threads;
	unsigned int itr = 0; // making the assumption that all tasks take the same amount of time on average

public:
	explicit ThreadManager(unsigned int numThreads = 3);

	void scheduleThread(std::shared_ptr<AbstractThreadTask> task);
	
};