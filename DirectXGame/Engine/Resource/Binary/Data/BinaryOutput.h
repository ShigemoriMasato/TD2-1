#pragma once
#include <ostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <Transform/Value.h>


class BinaryOutput {
public:

	void WriteBinary(std::ostream& out, const ValueBase* value);

private:

};
