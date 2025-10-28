#include "Coin.h"
#include <Resource/Audio/AudioManager.h>

void Coin::Initialize(ModelData* modelData, Camera* camera)
{
	BaseObject::Initialize(modelData, camera);
	collider_ = std::make_unique<SphereCollider>(
		ColliderTag::Dynamic,
		ColliderMask::COIN,
		ColliderMask::PLAYER);
	collider_->SetTransform(&transform_);
	
	soundHandle_ = audio_->Load("coin.mp3");
}

void Coin::Update(float deltaTime)
{
	modelResource_->position_ = transform_.position;
}

void Coin::OnCollision(BaseObject* other)
{
	if (other->GetCollider()->GetSelf() & ColliderMask::PLAYER)
	{
        audio_->Play(soundHandle_,false);
		isAlive_ = false;
	}
}
