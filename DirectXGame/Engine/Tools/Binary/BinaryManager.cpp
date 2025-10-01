#include "BinaryManager.h"
#include <fstream>

BinaryManager::BinaryManager() {
	input = std::make_unique<BinaryInput>();
	output = std::make_unique<BinaryOutput>();
}

BinaryManager::~BinaryManager() {
}

void BinaryManager::Write(std::string fileName) {
	// 書き出し専用の一時バッファを使う
	std::vector<std::shared_ptr<ValueBase>> buffer = std::move(values);
	values.clear();

	std::ofstream file(basePath + fileName, std::ios::binary);

	for (auto v : buffer) {
		output->WriteBinary(file, v.get());
	}

	file.close();
}

std::vector<std::shared_ptr<ValueBase>> BinaryManager::Read(std::string fileName) {

	std::ifstream file(basePath + fileName, std::ios::binary);
	if (!file.is_open()) {
		return {};
	}

	std::vector<std::shared_ptr<ValueBase>> ans;

	while(file.peek() != EOF) {
		auto val = input->ReadBinary(file);
		if (!val) {
			break;
		}
		ans.push_back(val);
	}

	file.close();

	return ans;
}
