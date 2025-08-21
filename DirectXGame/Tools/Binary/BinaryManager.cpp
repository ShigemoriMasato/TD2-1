#include "BinaryManager.h"
#include <fstream>
#include <filesystem>


namespace fs = std::filesystem;

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
	if (!file.is_open()) {
		throw std::runtime_error("Could not open file for writing: " + fileName);
	}

	for (auto v : buffer) {
		output->WriteBinaryFile(file, v.get());
	}

	file.close();
}

std::vector<std::shared_ptr<ValueBase>> BinaryManager::Read(std::string fileName) {

	std::ifstream file(basePath + fileName, std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("Could not open file for writing: " + fileName);
	}

	std::vector<std::shared_ptr<ValueBase>> ans;

	while(file.peek() != EOF) {
		auto val = input->ReadBinaryFile(file);
		if (!val) {
			break;
		}
		ans.push_back(val);
	}

	file.close();

	return ans;
}

void BinaryManager::MakeFile(std::string path) {
	fs::path filePath = fs::path(basePath) / path;
	// ディレクトリが存在しなければ作成
	fs::create_directories(filePath.parent_path());

	// ファイルが既に存在する場合は何もしない
	if (fs::exists(filePath)) {
		return;
	}

	// ファイルを作成
	std::ofstream ofs(filePath);
	if (!ofs) {
		throw std::runtime_error("Could not create file: " + filePath.string());
	}

	ofs.close();

	return;
}
