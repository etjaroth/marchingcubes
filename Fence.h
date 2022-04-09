#pragma once
#include "Config.h"

class Fence
{
	GLsync fence;
	bool active = false;

public:
	explicit Fence(bool active = false);
	Fence(const Fence& other) = delete;
	Fence& operator=(const Fence& other) = delete;
	~Fence();

	bool isDone() const;
	bool isActive() const;
	void waitUntilDone() const;

	void set();
	void release();
};

