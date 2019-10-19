#pragma once
class Timer
{
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
	std::string message;
public:
	Timer();
	~Timer();
	void start(const std::string& message);
	void stop();
};
