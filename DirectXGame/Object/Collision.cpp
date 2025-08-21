#include "Collision.h"

Collision::Collision(CollisionType collisionType, Object* object) {
	collisionType_ = collisionType;

	object_ = object;
}

void Collision::Update() {
	isHit_ = isHitMessage_;
	isHitMessage_ = false;
}

Object* Collision::GetObjectPtr() {
	return object_;
}

bool Collision::GetIsHit() {
	return isHit_;
}

RenderCollision::RenderCollision(CollisionType collisionType, Camera* camera, Object* object) : Collision(collisionType, object), Object(camera, ShapeType::Sphere) {
}

void RenderCollision::Update() {
	Collision::Update();

	//当たり判定のconfigを描画クラスに渡す
	transform_->position = *position_;
	
	switch (GetCollisionType()) {
	case CollisionType::Sphere:

		SetLocalPosition({ sphereConfig_.radius });

		break;

	case CollisionType::Capsule:

		SetLocalPosition({ capsuleConfig_.radius });

		break;
	}
}

void RenderCollision::SetColor(uint32_t color) {
	color_ = color;
}

CollisionType Collision::GetCollisionType() {
	return collisionType_;
}

void RenderCollision::SetCollisionType(CollisionType type) {
	Collision::SetCollisionType(type);

	//形の変更も行う
}
