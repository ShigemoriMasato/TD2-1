#include "Player.h"
#include "Wire.h"
#include <Core/EngineTerminal.h>
#include <algorithm>
#include <Game/Physics/PhysicsEngine.h>

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

Player::Player(TimeSlower* slower, PhysicsEngine* phEngine) {
	timeSlower_ = slower;
	actor_ = std::make_unique<PhysicsActor>(phEngine, this);
	collider_ = std::make_unique<SphereCollider>(
		ColliderTag::Dynamic,
		ColliderMask::PLAYER,
		ColliderMask::ENEMY | ColliderMask::ITEM);
	collider_->SetTransform(&transform_);
	collider_->SetSize(Vector3(1.0f, 1.0f, 1.0f));
}

Player::~Player()
{
}


void Player::Initialize(ModelData* modelData, Camera* camera) {
	BaseObject::Initialize(modelData,camera);
}

void Player::Update(float deltaTime)
{
	//更新

	modelResource_->position_ = transform_.position;

	RequestBehavior();
	(this->*behaviorUpdate[static_cast<int>(behavior_)])(deltaTime);
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
