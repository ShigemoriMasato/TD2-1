#include "EnemyBullet.h"
#include "Engine/Math/MyMath.h"
#include <numbers>

using namespace MyMath;
using namespace Matrix;

EnemyBullet::EnemyBullet(Camera* camera, Vector3 pos, int handle, Object* target) : Object(camera, ShapeType::Model) {
	handle_ = handle;
	transform_->position = pos;
	velocity_ = {};
	direction_ = Vector3(target->GetTransform().position - pos).Normalize();
	color = 0x502080ff;//青

	target_ = target;
	tag = "Enemy";
}

void EnemyBullet::Initialize() {
	transform_->scale = { 0.2f, 0.2f, 0.2f };
	velocity_ = direction_ * speed;
	//targetへ-Zが向くように調整
	float yaw = std::atan2(direction_.x, direction_.z);
	float pitch = std::atan2(-direction_.y, std::sqrt(direction_.x * direction_.x + direction_.z * direction_.z));

	transform_->rotation = { pitch, yaw, 0.0f };
}

void EnemyBullet::Update() {
	//向きの更新
	Vector3 nTrargetPos = Vector3(target_->GetTransform().position - transform_->position).Normalize();
	direction_ = Slerp(direction_, nTrargetPos, 0.1f);

	//移動処理
	velocity_ = direction_ * speed;
	transform_->position += velocity_;

	//targetへ-Zが向くように調整
	float yaw = std::atan2(direction_.x, direction_.z);
	float pitch = std::atan2(-direction_.y, std::sqrt(direction_.x * direction_.x + direction_.z * direction_.z));

	transform_->rotation = { pitch, yaw, 0.0f };

	//消滅処理
	++frame_;
	if (frame_ > 300) {
		isActive_ = false;
	}
}

void EnemyBullet::OnCollision(Object* other) {
	isActive_ = false;
}
