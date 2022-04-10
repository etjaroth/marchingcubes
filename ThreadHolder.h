#pragma once
#include "AbstractThreadTask.h"
#include <mutex>
#include <thread>
#include <queue>

class ThreadHolder
{
	std::shared_ptr<std::thread> myThread;

	std::mutex queueMutex;
	std::queue<std::shared_ptr<AbstractThreadTask>> tasks;

	bool running = true;
protected:
	virtual void run();

public:
	ThreadHolder();
	virtual ~ThreadHolder();
	void addTask(std::shared_ptr<AbstractThreadTask> newTask);
	void kill();
	bool isDone() const;
};

