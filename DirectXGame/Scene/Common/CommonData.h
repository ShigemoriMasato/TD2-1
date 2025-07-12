#pragma once
#include <vector>
#include "../../BeatManager.h"

enum class ModelType {

	ModelCount
};

enum class TextureType {

	TextureCount
};

struct CommonData {
	std::vector<int> modelHandle_;
	std::vector<int> textureHandle_;

	std::unique_ptr<BeatManager> beatManager_ = std::make_unique<BeatManager>();
};
