#pragma once
#include <vector>

enum class ModelType {
	SkySphere,
	Player,
	Enemy,
	Bullet,
	AccelerateGate,
	lowCommet,
	middleCommet,
	highCommet,
	Title,

	ModelCount
};

enum class TextureType {
	Reticle,

	TextureCount
};

struct CommonData {
	std::vector<int> modelHandle_;
	std::vector<int> textureHandle_;
};
