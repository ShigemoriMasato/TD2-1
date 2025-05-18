#pragma once
#include "MyMath.h"
#include "MyDirectX.h"
#include <vector>

class RainEmitter {
public:

	RainEmitter() = default;
	~RainEmitter() = default;

	void Initialize(Matrix4x4* vpMatrix, DirectionalLightData* dLightData);
	void Update(Transform& camera);
	void Draw(MyDirectX* md);

private:

	int coolTime_;
	std::vector<Transform> transforms_;
	const Transform initializeTransform_ = {0.002f, 0.35f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	const int kSamonAria_ = 7;
	const float kInitializePositiony_ = 5.0f;
	MaterialData material_;

	Matrix4x4* vpMatrix_;
	DirectionalLightData* dLightData_;

};

