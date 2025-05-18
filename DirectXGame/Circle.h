#pragma once
#include "MyMath.h"
#include "MyDirectX.h"

class Circle {
public:

	Circle() = default;
	~Circle() = default;
	void Initialize(Matrix4x4* vpMatrix, DirectionalLightData* dLightData, int division);
	void Update(Transform& transform, float radius, float thick);
	void Draw(MyDirectX* md);

	void SetMaterial(MaterialData material);

private:

	std::vector<Transform> transforms_;
	Transform mainTransform_{};
	MaterialData material_{};
	int division_ = 32;

	Matrix4x4* vpMatrix_ = nullptr;
	DirectionalLightData* dLightData_ = nullptr;
};

