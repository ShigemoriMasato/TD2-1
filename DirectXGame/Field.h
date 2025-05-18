#pragma once
#include "MyDirectX.h"
#include "MyMath.h"
#include "Circle.h"

class Field {
public:

	Field();
	~Field() = default;

	void Initialize(Matrix4x4* vpMatirx, DirectionalLightData* dLightData);
	void Update();
	void Draw(MyDirectX* md);

private:
	const int kCircleNum_ = 5;

	int frame_ = 0;

	std::vector<Circle> circles_;
	float theta_ = 0.0f;

	Matrix4x4* vpMatrix_ = nullptr;
	DirectionalLightData* dLightData_ = nullptr;
};
