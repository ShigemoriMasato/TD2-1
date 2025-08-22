#include "AccelerateGate.h"

int AccelerateGate::modelHandle_ = 0;

void AccelerateGate::SetHandle(int handle) {
	modelHandle_ = handle;
}

AccelerateGate::AccelerateGate(Camera* camera) : Object(camera, ShapeType::Model) {
	handle_ = modelHandle_;

	collision_ = std::make_shared<RenderCollision>(CollisionType::Sphere, camera, this);
	collision_->sphereConfig_.radius = 2.0f;
	collision_->tag_ = "middleBuff";
	collision_->SetColor(0xff);
}

void AccelerateGate::Update() {
	collision_->Update();
}

void AccelerateGate::Draw(const Matrix4x4* worldMatrix) const {
	Object::Draw();

	collision_->Draw();
}

void AccelerateGate::SetConfig(AccelerateGateConfig config) {
	*transform_ = config.transform;

}

AccelerateGateConfig AccelerateGate::GetConfig() const {
	AccelerateGateConfig config;
	
	config.transform = *transform_;


	return config;
}

void AccelerateGate::OnCollision(Object* other) {
	if (hited_) {
		return;
	}

	hited_ = true;

	collision_->Update();
}
