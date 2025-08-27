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
	C,
	l,
	e,
	a,
	r,
	Bikkuri,

	ModelCount
};

enum class TextureType {
	Reticle,
	t0,
	t1,
	t2,
	t3,
	t4,
	t5,
	t6,
	t7,
	t8,
	t9,

	TextureCount
};

struct CommonData {
	std::vector<int> modelHandle_;
	std::vector<int> textureHandle_;
};
