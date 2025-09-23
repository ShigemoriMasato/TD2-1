#pragma once
#include <ostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <Transform/Value.h>


class BinaryOutput {
public:

	void WriteVBin(std::ostream& out, const ValueBase* value);

private:

};
