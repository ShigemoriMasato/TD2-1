#pragma once
#include <istream>
#include <Data/Value.h>

class BinaryInput {
public:

	std::shared_ptr<ValueBase> ReadBinaryFile(std::istream& in);
};

