#pragma once
#include "Config.h"
#include <optional>
#include <queue>
#include <mutex>

template <class T>
class ThreadSafeQueue
{
	std::queue<T> queue;
	std::mutex mutex;

public:
	ThreadSafeQueue() = default;
	ThreadSafeQueue(const ThreadSafeQueue<T>&) = delete;
	ThreadSafeQueue& operator=(const ThreadSafeQueue<T>&) = delete;
	ThreadSafeQueue(ThreadSafeQueue<T>&& other) {
		std::lock_guard g(mutex);
		queue = std::move(other.queue);
	};

	void push(T item) {
		std::lock_guard g(mutex);
		queue.push(item);
	};

	std::optional<T> pop() {
		std::lock_guard g(mutex);
		
		if (queue.empty())
			return {};
		T temp = queue.front();
		queue.pop();

		return temp;
	};

	std::optional<T> popUnsafe() {
		if (queue.empty())
			return {};
		T temp = queue.front();
		queue.pop();

		return temp;
	};

	void stop() {
		mutex.lock();
	}

	void go() {
		mutex.unlock();
	}

	int size() {
		return queue.size();
	}
};

