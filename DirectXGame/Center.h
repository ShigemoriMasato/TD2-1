#pragma once
#include "MyMath.h"
#include "MyDirectX.h"

class Center {
public:

	Center() = default;
	~Center() = default;
	void Initialize(Matrix4x4* vpMatrix, DirectionalLightData* dLightData);
	void Update();
	void Draw(MyDirectX* md);

private:

	Transform largeBox_;
	float largeBoxScaleRate_ = 0.0f;

	Transform smallBox_[2];
	float smallBoxScaleRate_ = 0.0f;

	Transform sphere_;

	MaterialData largeMaterial_;
	MaterialData smallMaterial_;
	MaterialData sphereMaterial_;

	Matrix4x4* vpMatrix_;
	DirectionalLightData* dLightData_;

	int frame_ = 0;

};
