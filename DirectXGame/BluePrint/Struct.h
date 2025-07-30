#pragma once
#include <Data/Value.h>

class Struct {
public:
	Struct(std::string name, std::vector<ValueBase*> members)
		: name(std::move(name)), members(members) {}

	std::string name;
	std::vector<ValueBase*> members;
};