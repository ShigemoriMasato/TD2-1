#include "Player.h"
#include "Wire.h"
#include <Core/EngineTerminal.h>
#include <algorithm>

#ifdef max
#undef max
#endif

void (Player::* Player::behaviorUpdate[])(float) = {
	&Player::UpdateIdel,
	&Player::UpdateForcus,
	&Player::UpdateExtend,
	&Player::UpdateDash,
};

void (Player::* Player::behaviorOn[])() = {
	&Player::OnIdel,
	&Player::OnForcus,
	&Player::OnExtend,
	&Player::OnDash,
};

Player::Player(TimeSlower* slower) {
	timeSlower_ = slower;
}

Player::~Player()
{
}


void Player::Initialize(ModelData* modelData, Camera* camera) {
	BaseObject::Initialize(modelData,camera);

	radius_ = 1.0f;

	//debug
	transform_.scale = { 0.3f, 0.3f, 0.3f };
}

void Player::Update(float deltaTime)
{
	//更新

	modelResource_->position_ = transform_.position;

	RequestBehavior();
	(this->*behaviorUpdate[static_cast<int>(behavior_)])(deltaTime);

	transform_.position += velocity_ * deltaTime;
	transform_.position.y = std::max(transform_.position.y, 0.0f);
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

		(this->*behaviorOn[static_cast<int>(behavior_)])();

		behaviorRequest_ = std::nullopt;
	}
}
