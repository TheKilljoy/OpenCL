#include "pch.h"
#include "Timer.h"

Timer::Timer(){}

Timer::~Timer(){}

void Timer::start(const std::string& message)
{
	this->message = message;
	startTime = std::chrono::high_resolution_clock::now();
}

void Timer::stop()
{
	auto endTimepoint = std::chrono::high_resolution_clock::now();
	auto start = std::chrono::time_point_cast<std::chrono::microseconds>(startTime).time_since_epoch().count();
	auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

	auto duration = end - start;
	double ms = duration * 0.001;
	std::cout << message << " took " << duration << "us (" << ms << "ms)\n";
}
