#pragma once
#include <istream>
#include <Data/Value.h>

class BinaryInput {
public:

	std::shared_ptr<ValueBase> ReadVBin(std::istream& in);
};

