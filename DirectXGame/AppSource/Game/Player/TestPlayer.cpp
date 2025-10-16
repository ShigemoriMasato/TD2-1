#include "TestPlayer.h"

void TestPlayer::Initialize(ModelData* modelData, Camera* camera)
{
	BaseObject::Initialize(modelData, camera);
	collider_ = std::make_unique<SphereCollider>(
		ColliderTag::Dynamic,
		ColliderMask::PLAYER,
		ColliderMask::ENEMY | ColliderMask::ITEM);
	collider_->SetTransform(&transform_);
	transform_.position = { 5,5,0 };
}


void TestPlayer::Update(float deltaTime)
{
	modelResource_->position_ = transform_.position;

	if ((*key_)[Key::Right])actor_->force_.x += 100;
	if ((*key_)[Key::Left])actor_->force_.x -= 100;
	if ((*key_)[Key::Up])actor_->force_.y += 100;
	if ((*key_)[Key::Down])actor_->force_.y -= 100;
}

void TestPlayer::OnCollision(BaseObject* other)
{

}

