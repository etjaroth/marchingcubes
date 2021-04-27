#pragma once
#include "config.h"
class Stopwatch
{
	double start_time;
	double stop_time;
	bool running = false;
	bool paused = false;
	bool ever_started = false;

public:
	Stopwatch();
	void start();
	void pause();
	void stop();
	void reset();
	double get_time();
};

