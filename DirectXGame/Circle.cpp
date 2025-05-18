#include "Circle.h"

void Circle::Initialize(Matrix4x4* vpMatrix, DirectionalLightData* dLightData, int division) {
	vpMatrix_ = vpMatrix;
	dLightData_ = dLightData;
	division_ = division;
}

void Circle::Update(Transform& transform, float radius, float thick) {
	const float pie = 3.14159265358f;
	mainTransform_ = transform;
	transforms_.clear();
	float l = radius * 3.0f * pie;

	for (int i = 0; i < division_; ++i) {
		Transform tf = {};
		float theta = i * pie * 2.0f / float(division_);
		tf.scale = { l / division_, thick, 1.0f };
		tf.position = { radius * sinf(theta), 0.0f, radius * cosf(theta) };
		tf.rotation = { pie / 2.0f, theta, 0.0f };
		transforms_.push_back(tf);
	}
}

void Circle::Draw(MyDirectX* md) {
	for (const Transform& tf : transforms_) {
		Matrix4x4 worldMatrix = MakeTransformMatrix(tf) * MakeTransformMatrix(mainTransform_);
		md->DrawSprite3D({ -0.5f, 0.5f, 0.0f, 1.0f }, { 0.5f, 0.5f, 0.0f, 1.0f }, { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.5f, -0.5f, 0.0f, 1.0f }, worldMatrix * (*vpMatrix_), worldMatrix, material_, *dLightData_, 1);
	}
}

void Circle::SetMaterial(MaterialData material) {
	material_ = material;
}
