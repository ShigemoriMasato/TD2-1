#include "Player.h"
#include "Wire.h"
#include <Core/EngineTerminal.h>

Player::Player()
{
}

Player::~Player()
{
}

void Player::Initialize(ModelData* modelData, Camera* camera)
{
	BaseObject::Initialize(modelData, camera);

	radius_ = 1.0f;
}

void Player::Update(float deltaTime)
{
	//更新
	if ((*key_)[Key::Up])transform_.position.z		+= deltaTime * moveSpeed_;
	if ((*key_)[Key::Down])transform_.position.z	-= deltaTime * moveSpeed_;
	if ((*key_)[Key::Left])transform_.position.x	-= deltaTime * moveSpeed_;
	if ((*key_)[Key::Right])transform_.position.x	+= deltaTime * moveSpeed_;

}

void Player::Draw(Render* render)
{
	BaseObject::Draw(render);
	//if (wire_)
		//wire_->Draw(render);
	modelResource_->SetMaterial(0xffffffff, 1);

}

void Player::OnCollision(BaseObject* other)
{
	modelResource_->SetMaterial(0xff0000ff, 1);
}
