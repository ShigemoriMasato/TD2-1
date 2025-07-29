#pragma once
#include <vector>

enum class ModelType {
	MultiMesh,
	MultiMaterial,
	TeaPot,
	Suzanne,
	Bunny,

	ModelCount
};

enum class TextureType {
	SRT,

	TextureCount
};

struct CommonData {
	std::vector<int> modelHandle_;
	std::vector<int> textureHandle_;
};
