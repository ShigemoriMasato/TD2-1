#pragma once
#include <Data/Value.h>

template <typename T>
class Struct {
public:
	Struct(std::string name, std::vector<Value<T>> members)
		: name(std::move(name)), members(std::move(members)) {}

private:
	std::string name;										// 構造体の名前

};