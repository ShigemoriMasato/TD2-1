#include "Player.h"
#include "Engine/Input/Input.h"
#include "externals/imgui/imgui.h"
#include "Engine/Render/Render.h"
#include <algorithm>
#include <numbers>

Player::Player(Camera* camera, Camera* parent, CommonData* cd) : Object(camera, ShapeType::Model),
playerTransform_(std::make_shared<Transform>()),
parentCamera_(parent) {
	handle_ = cd->modelHandle_[int(ModelType::Player)];
	bulletModelHandle_ = cd->modelHandle_[int(ModelType::Bullet)];
	reticleHandle_ = cd->textureHandle_[int(TextureType::Reticle)];
	playerTransform_->rotation.y = std::numbers::pi_v<float>;
	tag = "Player";
}

void Player::Initialize() {
	playerTransform_->position = { 0.0f, 0.0f, 20.0f };
}

void Player::Update() {

	velocity_ = {};

	if (Input::GetKeyState(DIK_W)) {
		velocity_.y += 0.1f;
	}

	if (Input::GetKeyState(DIK_S)) {
		velocity_.y -= 0.1f;
	}

	if (Input::GetKeyState(DIK_A)) {
		velocity_.x -= 0.1f;
	}

	if (Input::GetKeyState(DIK_D)) {
		velocity_.x += 0.1f;
	}

	playerTransform_->position += velocity_;

	fpsCameraTransform_ = *playerTransform_;
	fpsCameraTransform_.rotation.y += std::numbers::pi_v<float>;
	fpsCameraTransform_.position.z += 0.1f;
	fpsCameraTransform_.position *= -1;
	fpsCameraTransform_.position.x += 0.1f;

	ImGui::Begin("player");
	ImGui::DragFloat3("Position", &playerTransform_->position.x, 0.1f);
	ImGui::End();

	playerTransform_->position.x = std::clamp(playerTransform_->position.x, -7.2f, 7.2f);
	playerTransform_->position.y = std::clamp(playerTransform_->position.y, -4.0f, 4.0f);

	if (cooltime_ > 0) {
		--cooltime_;
	}

	screenTransform_ = MakeAffineMatrix(*playerTransform_) *
		MakeScaleMatrix(parentCamera_->GetTransform().scale) *
		Inverse(MakeRotationMatrix(parentCamera_->GetTransform().rotation)) *
		MakeTranslationMatrix(parentCamera_->GetTransform().position);

	Vector3 pos = { screenTransform_.m[3][0], screenTransform_.m[3][1] , screenTransform_.m[3][2] };

	if (cooltime_ <= 0 && Input::GetKeyState(DIK_SPACE)) {
		cooltime_ = maxCooltime_;

		// プレイヤーの弾を発射
		std::shared_ptr<PlayerBullet> bullet = std::make_shared<PlayerBullet>(camera_, pos, Vector3(), bulletModelHandle_);

		bullet->Initialize();
		bullets_.push_back(bullet);
	}

	for (int i = 0; i < bullets_.size(); ++i) {
		bullets_[i]->Update();
		if (!bullets_[i]->GetIsActive()) {
			bullets_.erase(bullets_.begin() + i--);
		}
	}

	transform_->position = pos;

}

void Player::Draw(const Matrix4x4* worldMatrix) const {
	Render::DrawSprite(reticleWorldMatrix_, camera_, { 1.0f, 1.0f, 1.0f, 1.0f }, {}, reticleHandle_);

	Object::Draw(&screenTransform_);

	for (const auto& bullet : bullets_) {
		bullet->Draw();
	}
}

void Player::SetBulletTargetPosition(const Vector3* position) {
	bulletTargetPositions_.push_back(const_cast<Vector3*>(position));
}
