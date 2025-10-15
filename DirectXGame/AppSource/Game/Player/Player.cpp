#include "Player.h"
#include "Wire.h"
#include <Core/EngineTerminal.h>

Player::Player()
{
}

Player::~Player()
{
}


void Player::Initialize(ModelData* modelData, Camera* camera) {
	BaseObject::Initialize(modelData,camera);

	radius_ = 1.0f;
}

void Player::Update(float deltaTime)
{
	//更新
	if ((*key_)[Key::Up])transform_.position.z		+= deltaTime * moveSpeed_;
	if ((*key_)[Key::Down])transform_.position.z	-= deltaTime * moveSpeed_;
	if ((*key_)[Key::Left])transform_.position.x	-= deltaTime * moveSpeed_;
	if ((*key_)[Key::Right])transform_.position.x	+= deltaTime * moveSpeed_;

	modelResource_->position_ = transform_.position;
}

void Player::Draw(Render* render)
{
	BaseObject::Draw(render);
	if (wire_)
		wire_->Draw(render);
	modelResource_->color_ = 0xffffffff;
}

void Player::OnCollision(BaseObject* other)
{
	modelResource_->color_ = 0xff0000ff;
}

void Player::RequestBehavior()
{
	if (behaviorRequest_)
	{
		behaviorPrev_ = behavior_;
		behavior_ = behaviorRequest_.value();
		switch (behavior_)
		{
		case Behavior::Extend:
			break;
		case Behavior::Shrink:
			break;
		case Behavior::Idel:
			break;
		case Behavior::Hanging:
			break;
		case Behavior::Dash:
			break;
		}
		behaviorRequest_ = std::nullopt;
	}
}

void Player::UpdateBehavior(float deltaTime)
{
	switch (behavior_)
	{
	case Behavior::Extend:
		break;
	case Behavior::Shrink:
		break;
	case Behavior::Idel:
		break;
	case Behavior::Hanging:
		break;
	case Behavior::Dash:
		break;
	}
}
