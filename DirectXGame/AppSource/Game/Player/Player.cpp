#include "Player.h"
#include "Wire.h"
#include <Core/EngineTerminal.h>
#include <algorithm>
#include <Game/Physics/PhysicsEngine.h>

void (Player::* Player::behaviorUpdate[])(float) = {
	&Player::UpdateIdel,
	&Player::UpdateForcus,
	&Player::UpdateExtend,
	&Player::UpdateShrink,
	&Player::UpdateDash,
};

void (Player::* Player::behaviorOn[])() = {
	&Player::OnIdel,
	&Player::OnForcus,
	&Player::OnExtend,
	&Player::OnShrink,
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

	transform_.position = { 1.0f,1.0f,0.0f };
}

Player::~Player()
{
}

void Player::Initialize(ModelData* modelData, Camera* camera) {
	BaseObject::Initialize(modelData,camera);

	behaviorRequest_ = Behavior::Idle;
}

void Player::Update(float deltaTime)
{
	//更新

	modelResource_->position_ = transform_.position;


	RequestBehavior();
	(this->*behaviorUpdate[static_cast<int>(behavior_)])(deltaTime);

	wire_->Update(deltaTime);

	ImGui::Begin("Player Debug");
	ImGui::Text("Behavior: %d", static_cast<int>(behavior_));

	ImGui::Text("InWireField %d targets", (int)targets_.size());
	for (auto* t : targets_)
	{
		ImGui::Text("Target: %u", t->GetCollider()->GetSelf());
	}
	ImGui::End();
	targets_.clear();
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

Vector3 Player::GetInputDirection()
{
	Vector3 direction = { 0.0f, 0.0f, 0.0f };

	if (!key_) return direction;

	if ((*key_)[Key::Up]) direction.z += 1.0f;
	if ((*key_)[Key::Down]) direction.z -= 1.0f;
	if ((*key_)[Key::Left]) direction.x -= 1.0f;
	if ((*key_)[Key::Right]) direction.x += 1.0f;

	if (direction.Length() > 0.1f)
	{
		direction = direction.Normalize();
	}

	return direction;
}

BaseObject* Player::SelectTargetByDirection(const Vector3& direction)
{
	if (targets_.empty()) return nullptr;

	BaseObject* selectTarget = nullptr;
	float bestScore = -1.0f;
	const float maxAngle = 60.0f * ((float)std::numbers::pi / 180.0f);//60度の扇状の範囲

	for (auto it = targets_.begin(); it != targets_.end(); ++it)
	{
		auto target = (*it);
		if (!target) continue;

		Vector3 toTarget = (target->GetTransform()->position - transform_.position).Normalize();

		float angle = CalculateAngle(direction, toTarget);

		if (angle <= maxAngle)
		{
			float distance = (target->GetTransform()->position - transform_.position).Length();
			float distanceScore = 1.0f / (distance + 0.1f);
			float angleScore = 1.0f - (angle / maxAngle);

			float totalScore = distanceScore * 0.4f + angleScore * 0.6f;

			if (totalScore > bestScore)
			{
				bestScore = totalScore;
				selectTarget = target;
			}
		}
	}

	return selectTarget;
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
