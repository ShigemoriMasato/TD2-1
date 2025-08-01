#pragma once
#include <Data/Value.h>

class Struct {
public:
	Struct(std::vector<ValueBase*> members)
		: members(members) {
	}

	std::vector<ValueBase*> members; // 構造体のメンバー変数
};
