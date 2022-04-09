#pragma once
#include <atomic>

class AbstractThreadTask
{
	std::atomic_flag done;

public:
	AbstractThreadTask() { done.clear(); }
	virtual ~AbstractThreadTask() {}

	virtual void operator()() = 0;

	bool isDone() const {
		return done.test();
	}
};

template <class T>
class AbstractThreadTaskTemplate : public AbstractThreadTask {
protected:
	T data;

public:
	AbstractThreadTaskTemplate() {}
	virtual ~AbstractThreadTaskTemplate() {}

	T getData() const {
		return data;
	}
};
