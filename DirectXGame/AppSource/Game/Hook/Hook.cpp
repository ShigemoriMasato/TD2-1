#include "Hook.h"

void Hook::Initialize(ModelData* modelData, Camera* camera)
{
	BaseObject::Initialize(modelData, camera);
	collider_ = std::make_unique<SphereCollider>(
		ColliderTag::Static,
		ColliderMask::HOOK,
		ColliderMask::PLAYER);
	collider_->SetTransform(&transform_);
}

void Hook::Update(float deltaTime)
{
	modelResource_->position_ = transform_.position;
}
