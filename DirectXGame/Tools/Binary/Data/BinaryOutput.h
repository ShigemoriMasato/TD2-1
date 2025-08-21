#pragma once
#include <ostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <Data/Value.h>


class BinaryOutput {
public:

	void WriteBinaryFile(std::ostream& out, const ValueBase* value);

private:

};
