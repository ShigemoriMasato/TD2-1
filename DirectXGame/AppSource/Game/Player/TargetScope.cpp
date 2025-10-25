#include "TargetScope.h"
#include "Player.h"
#include <numbers>

TargetScope::TargetScope() {
	scope_ = std::make_unique<DrawResource>();
	scope_->Initialize(ShapeType::Plane);

	sphere_ = std::make_unique<DrawResource>();
	sphere_->Initialize(ShapeType::Sphere);
	sphere_->psoConfig_.rasterizerID = RasterizerID::Wireframe;
}

TargetScope::~TargetScope() {
}

void TargetScope::Initialize(int textureHandle, Player* player, Camera* camera) {
	scope_->SetTextureHandle(textureHandle);
	playerPosition_ = &player->GetTransform()->position;
	scope_->scale_ = { 8.0f,8.0f,1.0f };
	scope_->camera_ = camera;
	sphere_->scale_ = { 1.3f,1.3f,0.5f };
	sphere_->camera_ = camera;
	player_ = player;
}

void TargetScope::Update(float deltaTime, KeyManager* keyManager) {
	if (player_->GetBehavior() == Player::Behavior::Forcus) {

		//プレイヤーの方向を取得
		auto key = keyManager->GetKeyStates();
		Vector2 direction = { 0.0f, 0.0f };
		float rotate = 0.0f;

		if ((key)[Key::Up]) direction.y += 1.0f;
		if ((key)[Key::Down]) direction.y -= 1.0f;
		if ((key)[Key::Left]) direction.x -= 1.0f;
		if ((key)[Key::Right]) direction.x += 1.0f;

		//raddiunに直す
		if (direction.Length() > 0.1f) {
			scope_->color_ = 0xffffffff;
			
			if (direction.x == 1) {
				if (direction.y == 1) {
					rotate = 0.25f;
				} else if (direction.y == -1) {
					rotate = 1.75f;
				} else {
					rotate = 0.0f;
				}
			} else if (direction.x == -1) {
				if (direction.y == 1) {
					rotate = 0.75f;
				} else if (direction.y == -1) {
					rotate = 1.25f;
				} else {
					rotate = 1.0f;
				}
			} else {
				if (direction.y == 1) {
					rotate = 0.5f;
				} else if (direction.y == -1) {
					rotate = 1.5f;
				} else {
					rotate = 0.0f;
				}
			}

			rotate *= std::numbers::pi_v<float>; //ラジアンに変換

			scope_->position_ = *playerPosition_;
			scope_->rotate_ = { 0.0f, 0.0f, rotate };

			scope_->SetWorldMatrix(MakeScaleMatrix(scope_->scale_) * MakeTranslationMatrix(offset_) * MakeRotationMatrix(scope_->rotate_) * MakeTranslationMatrix(scope_->position_));

			sphere_->color_ = 0xd02060ff;
			sphere_->position_ = player_->GetTargetPos();

			if(sphere_->position_.Length() < 0.1f) {
				sphere_->color_ = 0;
			}

		} else {
			scope_->color_ = 0;
			sphere_->color_ = 0;
		}

	} else {
		scope_->color_ = 0;
		sphere_->color_ = 0;
	}

	ImGui::Begin("Scope");
	ImGui::DragFloat3("Offset", &offset_.x, 0.01f);
	ImGui::DragFloat3("Scale", &scope_->scale_.x, 0.01f);
	ImGui::End();
}

void TargetScope::Draw(Render* render) {
	render->Draw(scope_.get());
	render->Draw(sphere_.get());
}
