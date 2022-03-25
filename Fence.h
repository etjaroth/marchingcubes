#pragma once
#include "Config.h"

class Fence
{
	GLsync fence;
	bool active = false;

public:
	Fence(bool active = false);
	~Fence();

	bool isDone();
	void set();
	void release();
};

