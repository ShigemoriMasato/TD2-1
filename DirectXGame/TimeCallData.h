#pragma once
#include <functional>

struct TimeCallData {
	std::function<void()> func;
	int executeFrame = 0;
	int registerFrame = 0;
	bool repeat = false;
};
