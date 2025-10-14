#include "FloatingEnemy.h"
#include <Tools/FPS/FPSObserver.h>
#include <cmath>

FloatingEnemy::FloatingEnemy() {
}

void FloatingEnemy::Initialize(ModelData* modelData, Camera* camera) {
	// BaseObjectのInitializeを呼び出してモデルリソースを初期化
	BaseObject::Initialize(modelData, camera);

	// 敵の初期設定（BunnyEnemy2の右隣に配置）
	transform_.position = { 6.0f, 0.0f, 5.0f }; // さらに右に配置
	transform_.rotation = { 0.0f, 0.0f, 0.0f };
	transform_.scale = { 1.0f, 1.0f, 1.0f };

	// 浮遊効果の初期値設定
	baseHeight_ = transform_.position.y;
	currentHeight_ = baseHeight_;
	targetHeight_ = baseHeight_;

	isActive_ = true;

	// ModelResourceのTransformを更新
	if (modelResource_) {
		modelResource_->scale_ = transform_.scale;
		modelResource_->rotate_ = transform_.rotation;
		modelResource_->position_ = transform_.position;

	}
}

void FloatingEnemy::Update(float deltaTime) {
	if (!isActive_) return;

	// プレイヤーが範囲内にいるかチェックして目標高度を設定
	if (IsPlayerInRange()) {
		// プレイヤーが近くにいる場合、浮上
		targetHeight_ = baseHeight_ + floatHeight_;
	} else {
		// プレイヤーが離れている場合、元の高さに戻る
		targetHeight_ = baseHeight_;
	}

	// 浮遊効果を更新
	UpdateFloatingEffect(deltaTime);

	// Y座標を更新
	transform_.position.y = currentHeight_;

	// ModelResourceのTransformを更新
	if (modelResource_) {
		modelResource_->scale_ = transform_.scale;
		modelResource_->rotate_ = transform_.rotation;
		modelResource_->position_ = transform_.position;

	}

}

void FloatingEnemy::Draw(Render* render) {
	if (!isActive_) return;

	// BaseObjectのDrawを使用
	BaseObject::Draw(render);
}

bool FloatingEnemy::IsPlayerInRange() const {
	float distance = GetDistanceToPlayer();
	return distance <= effectRange_;
}

float FloatingEnemy::GetDistanceToPlayer() const {
	// プレイヤーとの水平距離を計算（Y軸は除外）
	float deltaX = playerPosition_.x - transform_.position.x;
	float deltaZ = playerPosition_.z - transform_.position.z;
	return std::sqrt(deltaX * deltaX + deltaZ * deltaZ);
}

void FloatingEnemy::UpdateFloatingEffect(float deltaTime) {
	// 目標高度に向かって滑らかに移動
	float heightDifference = targetHeight_ - currentHeight_;

	// 高度差が小さい場合は直接設定
	if (std::abs(heightDifference) < 0.01f) {
		currentHeight_ = targetHeight_;
	} else {
		// 指数関数的に目標に近づく（イージング効果）
		currentHeight_ += heightDifference * floatSpeed_ * deltaTime;
	}
}