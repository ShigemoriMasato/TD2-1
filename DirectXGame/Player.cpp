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

	if (Input::GetKeyState(DIK_F) && !Input::GetPreKeyState(DIK_F)) {
		isStalking_ = !isStalking_;
	}

	if (!isStalking_) {
		bulletTargetPositions_.clear();
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

	//弾を撃つ方向を定める
	Vector3 target = Vector3();
	bool isFindReticleTarget = false;

	//ターゲットがいたら
	if (bulletTargetPositions_.empty()) {

		target = { reticleWorldMatrix_.m[3][0], reticleWorldMatrix_.m[3][1], reticleWorldMatrix_.m[3][2] };

		//いなかったら
	} else {

		isFindReticleTarget = true;

		int i = 0;
		bool isFound = false;
		//見つかるまで探索
		while (!isFound) {
			for (auto& pos : bulletTargetPositions_) {
				if (i++ == reticleIndex_) {
					target = *pos;
					isFound = true;
					break;
				}
			}

			if (!isFound) {
				reticleIndex_ = 0;
				i = 0;
			}
		}
	}

	if (cooltime_ <= 0 && Input::GetKeyState(DIK_SPACE)) {
		cooltime_ = maxCooltime_;

		// プレイヤーの弾を発射
		std::shared_ptr<PlayerBullet> bullet = std::make_shared<PlayerBullet>(camera_, pos, target, bulletModelHandle_);

		++reticleIndex_;

		if (reticleIndex_ == 2) {
			reticleHandle_ = reticleHandle_;
		}

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

	//コントローラーorマウス
	if (isXBoxController_) {
		Vector2 stick = Input::GetXBoxStickState(1); // 1:左スティック
		Vector2 velocity = stick * reticleSpeed_ * reticleMoveAdjustment_;
		reticleTransform_.position.x += velocity.x;
		reticleTransform_.position.y += velocity.y;
	} else {
		ImGuiIO& io = ImGui::GetIO();
		Vector2 mousePos = { io.MousePos.x, io.MousePos.y };
		reticleTransform_.position = { mousePos.x / 1280.0f - 0.5f, -mousePos.y / 720.0f + 0.5f, 30.0f };
		reticleTransform_.position *= Vector3(12.0f * 1.75, 12.0f, 1.0f);
	}

	//ターゲットが見つかっていなければ上のやつを適用
	if (!isFindReticleTarget) {
		reticleTransform_.position.x = std::clamp(reticleTransform_.position.x, -10.5f, 10.5f);
		reticleTransform_.position.y = std::clamp(reticleTransform_.position.y, -6.0f, 6.0f);

		reticleWorldMatrix_ = MakeAffineMatrix(reticleTransform_) *
			MakeScaleMatrix(parentCamera_->GetTransform().scale) *
			Inverse(MakeRotationMatrix(parentCamera_->GetTransform().rotation)) *
			MakeTranslationMatrix(parentCamera_->GetTransform().position);

	} else {

		//見つかっているのでターゲットにポジションを移す
		reticleTransform_.position = target;

		reticleWorldMatrix_ = MakeAffineMatrix(reticleTransform_);
	}

	ImGui::Begin("WorldPlayer");
	ImGui::Text("Reticle Position: (%.2f, %.2f, %.2f)", reticleTransform_.position.x, reticleTransform_.position.y, reticleTransform_.position.z);
	ImGui::DragFloat("Reticle Move Adjustment", &reticleMoveAdjustment_, 0.01f);
	ImGui::End();

	bulletTargetPositions_.clear(); // 弾のターゲット位置をクリア
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
