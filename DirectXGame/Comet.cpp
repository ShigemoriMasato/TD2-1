#include "Comet.h"

Comet::Comet(Camera* camera, std::array<int, 3> modelHandle, Vector3* playerPositionPtr) : Object(camera, ShapeType::Model) {
	modelHandle_ = modelHandle;

	handle_ = modelHandle_[2];

	playerPosition_ = playerPositionPtr;

	tag_ = "Enemy";

	size_.x = 1.0f;

	mainCollision_ = std::make_shared<RenderCollision>(CollisionType::Sphere, camera, this);
	nearCollision_ = std::make_shared<RenderCollision>(CollisionType::Sphere, camera, this);
}

void Comet::Initialize() {

}

void Comet::Update() {
	
	float distance = (transform_->position - *playerPosition_).Length();

	if (distance > 500.0f) {
		handle_ = modelHandle_[0]; // low Commet
	} else if(distance > 300.0f) {
		handle_ = modelHandle_[0]; // Middle Commet
	} else {
		handle_ = modelHandle_[0]; // high Commet
	}


}

void Comet::SetPosition(const Vector3& position) {
	transform_->position = position;
}

void Comet::AddMovement(const Vector3& movement) {
	transform_->position += movement;
}

void Comet::OnCollision(Object* other) {
	
}

CometConfig Comet::GetConfig() const {
	CometConfig config;
	config.pos = transform_->position;
	return config;
}

void Comet::SetConfig(const CometConfig& config) {
	transform_->position = config.pos;
}

Vector3* Comet::GetPositionPtr() const {
	return &transform_->position;
}

void Comet::Draw(const Matrix4x4* worldMatrix) const {

}
