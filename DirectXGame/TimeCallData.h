#pragma once
#include <functional>

struct TimeCallData {
	std::function<void()> func;
	float value1 = FLT_MIN;
	float value2 = FLT_MIN;
	float value3 = FLT_MIN;
	int executeFrame = 0;
	int registerFrame = 0;
	int line = 0;
	int id = 0;
	bool repeat = false;
};
