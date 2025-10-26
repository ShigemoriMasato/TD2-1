#include "Player.h"
#include "Wire.h"
#include <Core/EngineTerminal.h>
#include <algorithm>
#include <Game/Physics/PhysicsEngine.h>

void (Player::*Player::behaviorUpdate[])(float) = {
	&Player::UpdateIdel,
	&Player::UpdateForcus,
	&Player::UpdateExtend,
	&Player::UpdateShrink,
	&Player::UpdateDash,
	& Player::UpdateClear,
};

void (Player::*Player::behaviorOn[])() = {
	&Player::OnIdel,
	&Player::OnForcus,
	&Player::OnExtend,
	&Player::OnShrink,
	&Player::OnDash,
	&Player::OnClear,
};

Player::Player(TimeSlower* slower, PhysicsEngine* phEngine)
{
	timeSlower_ = slower;
	actor_ = std::make_unique<PhysicsActor>(phEngine, this);
	collider_ = std::make_unique<SphereCollider>(
		ColliderTag::Dynamic,
		ColliderMask::PLAYER,
		ColliderMask::ENEMY |
		ColliderMask::GOAL |
		ColliderMask::HOOK |
		ColliderMask::COIN);

	collider_->SetTransform(&transform_);
	collider_->SetSize(Vector3(1.0f, 1.0f, 1.0f));

	transform_.position = { 1.0f,1.0f,0.0f };
}

Player::~Player()
{
}

void Player::Initialize(ModelData* modelData, Camera* camera)
{
	BaseObject::Initialize(modelData, camera);

	behaviorRequest_ = Behavior::Idle;
}

void Player::Update(float deltaTime)
{
	//更新
	modelResource_->position_ = transform_.position;

	transform_.rotation.z = std::fmodf(transform_.rotation.z + rotateSpeed_ * deltaTime, std::numbers::pi_v<float> * 2.0f);
	modelResource_->rotate_ = transform_.rotation;

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

}

void Player::OnCollision(BaseObject* other)
{

}

Vector3 Player::GetInputDirection()
{
	if (!key_) return direction;
	direction = { 0.0f,0.0f,0.0f };
	if ((*key_)[Key::Up]) direction.y += 1.0f;
	if ((*key_)[Key::Down]) direction.y -= 1.0f;
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
	const float allowAngle = std::cos(0.125f * std::numbers::pi_v<float>);	//判定をとりうる範囲(これより値が小さければターゲットとして選定する)

	for (auto target : targets_)
	{
		if (!target) continue;

		Vector3 targetToPlayer = (target->GetTransform()->position - transform_.position);
		float angle = MyMath::dot(direction, targetToPlayer.Normalize());


		//ターゲットとプレイヤーの間に障害物がないか
		if (!tileMap_->HasTile(this->transform_.position, target->GetTransform()->position, TileType::Solid))
		{
			//許容角度内にいるか
			if (angle > allowAngle)
			{
				//一番ターゲットが狙いやすいと思われるオブジェクトを選ぶ
				float distance = (target->GetTransform()->position - transform_.position).Length();
				float distanceScore = 1.0f / (distance + 0.1f);
				float angleScore = -angle;

				float totalScore = distanceScore * 0.4f + angleScore * 0.6f;

				if (totalScore > bestScore)
				{
					bestScore = totalScore;
					selectTarget = target;
				}
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
