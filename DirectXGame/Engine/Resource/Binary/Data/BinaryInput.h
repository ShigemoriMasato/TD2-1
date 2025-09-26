#pragma once
#include <istream>
#include <Transform/Value.h>

class BinaryInput {
public:

	std::shared_ptr<ValueBase> ReadBinary(std::istream& in);
};

