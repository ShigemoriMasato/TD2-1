#include "TargetScope.h"
#include "Player.h"
#include <numbers>

TargetScope::TargetScope() {
	scopeTop_ = std::make_unique<DrawResource>();
	scopeTop_->Initialize(ShapeType::Plane);
	scopeTop_->localPos_ = {
		{3.0f, 0.1f, 0.0f},
		{10.0f, 0.1f, 0.0f},
		{3.0f, -0.1f, 0.0f},
		{10.0f, -0.1f, 0.0f},
	};
	scopeTop_->texcoord_ = {
		{0.0f, 0.0f},
		{2.0f, 0.0f},
		{0.0f, 1.0f},
		{2.0f, 1.0f},
	};

	scopeBelow_ = std::make_unique<DrawResource>();
	scopeBelow_->Initialize(ShapeType::Plane);
	scopeBelow_->localPos_ = scopeTop_->localPos_;
	scopeBelow_->texcoord_ = scopeTop_->texcoord_;

	sphere_ = std::make_unique<DrawResource>();
	sphere_->Initialize(ShapeType::Sphere);
	sphere_->psoConfig_.rasterizerID = RasterizerID::Wireframe;
}

TargetScope::~TargetScope() {
}

void TargetScope::Initialize(int textureHandle, Player* player, Camera* camera) {
	playerPosition_ = &player->GetTransform()->position;
	scopeTop_->camera_ = camera;
	scopeBelow_->camera_ = camera;
	scopeTop_->SetTextureHandle(textureHandle);
	scopeBelow_->SetTextureHandle(textureHandle);
	sphere_->scale_ = { 1.3f,1.3f,0.5f };
	sphere_->camera_ = camera;
	player_ = player;
}

void TargetScope::Update(float deltaTime, KeyManager* keyManager) {
	if (player_->GetBehavior() == Player::Behavior::Forcus) {

		Vector3 dir = (player_->GetDirection()).Normalize();
		float angle = std::atan2f(dir.y, dir.x);
		
		scopeTop_->rotate_.z = angle + std::numbers::pi_v<float> / 8.0f;
		scopeBelow_->rotate_.z = angle - std::numbers::pi_v<float> / 8.0f;

		scopeTop_->position_ = *playerPosition_;
		scopeBelow_->position_ = *playerPosition_;

		scopeTop_->color_ = 0xffffffff;
		scopeBelow_->color_ = 0xffffffff;
	} else {
		scopeTop_->color_ = 0;
		scopeBelow_->color_ = 0;
	}

	scopeTop_->texturePos_.x -= deltaTime / 2.0f;
	scopeBelow_->texturePos_.x -= deltaTime / 2.0f;
}

void TargetScope::Draw(Render* render) {
	if (player_->GetBehavior() == Player::Behavior::Forcus) {
		render->Draw(scopeTop_.get());
		render->Draw(scopeBelow_.get());
		render->Draw(sphere_.get());
	}
}
