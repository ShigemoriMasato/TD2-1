#include "Comet.h"

Comet::Comet(Camera* camera, CommonData* commonData, Vector3* playerPositionPtr) : Object(camera, ShapeType::Model) {
	modelHandle_ = {
		commonData->modelHandle_[int(ModelType::lowCommet)],
		commonData->modelHandle_[int(ModelType::middleCommet)],
		commonData->modelHandle_[int(ModelType::highCommet)]
	};

	handle_ = modelHandle_[2];

	playerPosition_ = playerPositionPtr;

	tag = "Enemy";
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
