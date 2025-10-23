#include "Coin.h"

void Coin::Initialize(ModelData* modelData, Camera* camera)
{
	BaseObject::Initialize(modelData, camera);
	collider_ = std::make_unique<SphereCollider>(
		ColliderTag::Dynamic,
		ColliderMask::COIN,
		ColliderMask::PLAYER);
	collider_->SetTransform(&transform_);
}

void Coin::Update(float deltaTime)
{
	modelResource_->position_ = transform_.position;
}

void Coin::OnCollision(BaseObject* other)
{
	if (other->GetCollider()->GetSelf() & ColliderMask::PLAYER)
	{
		isAlive_ = false;
	}
}
