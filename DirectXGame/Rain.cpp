#include "Rain.h"

void RainEmitter::Initialize(Matrix4x4* vpMatrix, DirectionalLightData* dLightData) {
	vpMatrix_ = vpMatrix;
	dLightData_ = dLightData;
	transforms_.clear();
	material_.color = { 0.2f, 0.2f, 0.2f, 1.0f };
	material_.enableLighting = true;
}

void RainEmitter::Update(Transform& camera) {
	if (coolTime_ > 0) {
		--coolTime_;
	} else {
		coolTime_ = 0;
		Transform transform = initializeTransform_;
		transform.position = { (rand() % (kSamonAria_ * 100) - (kSamonAria_ * 50)) / 100.0f, kInitializePositiony_, (rand() % (kSamonAria_ * 100) - (kSamonAria_ * 50)) / 100.0f};
		transforms_.push_back(transform);
	}

	for (Transform& tf : transforms_) {
		tf.position.y -= 0.3f;
		tf.rotation.y = camera.rotation.y;
		if (tf.position.y < -3.0f) {
			transforms_.erase(transforms_.begin());
		}
	}
}

void RainEmitter::Draw(MyDirectX* md) {
	for (const Transform& tf : transforms_) {
		md->DrawSprite3D({ -0.5f, 0.5f, 0.0f, 1.0f }, { 0.5f, 0.5f, 0.0f, 1.0f }, {-0.5f, -0.5f, 0.0f, 1.0f}, {0.5f, -0.5f, 0.0f, 1.0f}, MakeTransformMatrix(tf), MakeTransformMatrix(tf) * (*vpMatrix_), material_, *dLightData_, 1);
	}
}
