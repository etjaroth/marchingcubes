#include "ThreadManager.h"

unsigned int ThreadManager::timestamp = 0;

ThreadManager::ThreadManager(unsigned int numThreads)
	: numThreads{ numThreads } {

}

ThreadManager::~ThreadManager() {

}

void ThreadManager::scheduleThread(std::shared_ptr<AbstractThreadTask> task) {
	
}

void ThreadManager::iterate() {
	for (auto itr = activeThreads.begin(); itr != activeThreads.end();) {
		if (itr->first->isDone()) {
			itr->second.join();
			itr = activeThreads.erase(itr);
		}
		else {
			++itr;
		}
	}

	while (activeThreads.size() < numThreads && !pqueue.empty()) {
		std::shared_ptr<AbstractThreadTask> task = pqueue.top().second;
		pqueue.pop();
		//activeThreads.insert({ task, std::thread(*task) });
	}
}