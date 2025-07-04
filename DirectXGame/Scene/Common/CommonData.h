#pragma once
#include <vector>

enum class ModelType {

	ModelCount
};

enum class TextureType {

	TextureCount
};

struct CommonData {
	std::vector<int> modelHandle_;
	std::vector<int> textureHandle_;
};
