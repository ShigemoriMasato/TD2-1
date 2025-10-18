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

	if ((*key_)[Key::Right])actor_->force_.x = 100;
	if ((*key_)[Key::Left])actor_->force_.x = -100;
	if ((*key_)[Key::Up])actor_->force_.y = 100;
	if ((*key_)[Key::Down])actor_->force_.y = -100;

	ImGui::Text("top %s", actor_->collidedTop_ ? "true" : "false");
    ImGui::Text("bottom %s", actor_->collidedBottom_ ? "true" : "false");
    ImGui::Text("left %s", actor_->collidedLeft_ ? "true" : "false");
    ImGui::Text("right %s", actor_->collidedRight_ ? "true" : "false");

}

void TestPlayer::OnCollision(BaseObject* other)
{

}

