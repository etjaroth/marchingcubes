#include "ThreadManager.h"
#include <iostream>
#include "HeightmapMeshGenerator.h"



ThreadManager::ThreadManager(unsigned int numThreads) {
	for (unsigned int i = 0; i < numThreads; ++i) {
		threads.push_back(std::make_shared<ThreadHolder>());
	}
}

void ThreadManager::scheduleThread(std::shared_ptr<AbstractThreadTask> task) {
	itr = (itr + 1) % threads.size();
	threads[itr]->addTask(task);
}
