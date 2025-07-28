#pragma once
#include <Data/Value.h>

struct Function {
public:

	Function(std::string name, std::vector<Value> args, Value returnValue)
		: name(name), args(std::move(args)), returnValue(std::move(returnValue)) {}


private:

	std::string name;
	std::vector<Value> args;
	Value returnValue;

};
