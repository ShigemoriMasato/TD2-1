#include "AccelerateGate.h"

int AccelerateGate::modelHandle_ = 0;

void AccelerateGate::SetHandle(int handle) {
	modelHandle_ = handle;
}

AccelerateGate::AccelerateGate(Camera* camera, Vector3 position, Vector3 rotation) : Object(camera, ShapeType::Model) {
	handle_ = modelHandle_;
	transform_->position = position;
	transform_->rotation = rotation;
	tag = "Accelerate";
}

void AccelerateGate::OnCollision(Object* other) {
	if (hited_) {
		return;
	}

	hited_ = true;
}
