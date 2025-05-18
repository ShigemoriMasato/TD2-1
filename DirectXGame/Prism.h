#pragma once
#include "MyMath.h"
#include "MyDirectX.h"
#include <vector>
#include "Circle.h"

class Prism {
public:
	Prism();
	~Prism();

	enum Kind {
		kinner,
		kround,
		kouter
	};

	void Initialize(Matrix4x4* vpMatrix, DirectionalLightData* dLightData, Vector4 color, float radius, int prismNum);
	
	void Update(Kind direction);
	
	void Draw(MyDirectX* md);

private:

	Circle* circle_;

	int prismNum_ = 32;

	const float kRatioPrismToSphere_ = 50.0f;

	float radius_ = 0.0f;

	Vector4 lineVertexlist_[5]{};
	std::vector<Transform> transforms_;
	std::vector<float> positionAngle_;
	MaterialData material_;
	int textureHandle_ = 1;

	DirectionalLightData* dLightData_;
	Matrix4x4* vpMatrix_;

};
