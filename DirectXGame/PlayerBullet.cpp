#include "PlayerBullet.h"
#include "Engine/Math/MyMath.h"

using namespace Matrix;

PlayerBullet::PlayerBullet(Camera* camera, Vector3 pos, Vector3 target, int handle) : Object(camera, ShapeType::Model) {
	handle_ = handle;
	transform_->position = pos;
	
	velocity_ = {};
	direction_ = target.Normalize();
	tag = "Player";
	color = 0x888800ff;
}

void PlayerBullet::Initialize() {
	transform_->scale = { 0.3f, 0.3f, 0.3f };
	velocity_ = direction_ * speed;
}

void PlayerBullet::Update() {
	transform_->position += velocity_;

	if (++frame_ > 300) {
		isActive_ = false;
	}
}

void PlayerBullet::OnCollision(Object* other) {
	if (other->tag == "Enemy") {
		isActive_ = false;
	}
}
