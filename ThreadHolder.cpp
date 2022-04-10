#include "ThreadHolder.h"

void ThreadHolder::run() {
	std::shared_ptr<AbstractThreadTask> task = nullptr;
	while (running) {
		if (task == nullptr || task->isDone()) {
			std::lock_guard g(queueMutex);
			if (!tasks.empty()) {
				task = tasks.front();
				tasks.pop();
				(*task)();
			}
		}
	}
}

ThreadHolder::ThreadHolder()
	: myThread{std::make_shared<std::thread>(&ThreadHolder::run, this)} {

}

ThreadHolder::~ThreadHolder() {
	kill();
}

void ThreadHolder::addTask(std::shared_ptr<AbstractThreadTask> newTask) {
	std::lock_guard g(queueMutex);
	tasks.push(newTask);
}

void ThreadHolder::kill() {
	// not atomic but it should be fine
	// (assuming it's ok to run an extra task before deletion)
	running = false;
	if (myThread && myThread->joinable()) {
		myThread->join();
	}
	myThread = nullptr;
}

bool ThreadHolder::isDone() const {
	// not atomic but it should be fine
	// since running is false iff the thread has been killed
	// and there is no way to set running to true again
	return running;
}