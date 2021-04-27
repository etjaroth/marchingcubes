#include "Stopwatch.h"

Stopwatch::Stopwatch() {
	paused = false;
	start();
}

void Stopwatch::start() {
	if (!paused) {
		reset();
	}
	running = true;
}

void Stopwatch::stop() {
	stop_time = glfwGetTime();
	running = false;
}

void Stopwatch::reset() {
	start_time = glfwGetTime();
}

double Stopwatch::get_time() {
	if (running) {
		return glfwGetTime() - start_time;
	}
	else {
		return stop_time - start_time;
	}
}