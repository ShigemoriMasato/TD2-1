#include "SplitEnemy.h"
#include "../EnemySpawnParams.h"
#include <Tools/FPS/FPSObserver.h>
#include <Common/KeyConfig/KeyManager.h>

SplitEnemy::SplitEnemy() {
}


void SplitEnemy::Initialize(ModelData* modelData, Camera* camera) {
	// BaseObjectのInitializeを呼び出してモデルリソースを初期化
	BaseObject::Initialize(modelData, camera);

	transform_.position = { 0.0f, 0.0f, 0.0f };
	transform_.rotation = { 0.0f, 0.0f, 0.0f };
	transform_.scale = { 0.7f, 0.7f, 0.7f }; // 元の敵より小さくする

	isActive_ = true;
	isAlive_ = true;
	isDead_ = false;

	// 初期Transform設定をModelResourceに反映
	if (modelResource_) {
		modelResource_->scale_ = transform_.scale;
		modelResource_->rotate_ = transform_.rotation;
		modelResource_->position_ = transform_.position;
	}
}

void SplitEnemy::Update(float deltaTime) {
	if (!isAlive_) return;

	// 左右で異なる死亡条件
	if (isLeftEnemy_) {
		// 左の敵：1秒経過で死亡
		lifeTime_ += deltaTime;
		if (lifeTime_ >= 1.0f) {
			Die();
			return;
		}
	} else {
		// 右の敵：3秒経過で死亡（スペースキーによる即死を防ぐ）
		lifeTime_ += deltaTime;
		if (lifeTime_ >= 3.0f) {
			Die();
			return;
		}
	}

	// 分裂後の敵は左右に移動する
	transform_.position.x += moveDirection_ * moveSpeed_ * deltaTime;

	// 回転させる（元の敵より速く）
	transform_.rotation.y += 2.0f * deltaTime;

	// ModelResourceのTransformを更新
	if (modelResource_) {
		modelResource_->scale_ = transform_.scale;
		modelResource_->rotate_ = transform_.rotation;
		modelResource_->position_ = transform_.position;
	}
}

void SplitEnemy::Draw(Render* render) {
	if (!isActive_ || !isAlive_) return;
	BaseObject::Draw(render);
}

void SplitEnemy::Die() {
	if (!isAlive_) return;

	isDead_ = true;
	isAlive_ = false;  // 生存フラグをfalseに

}

void SplitEnemy::SetPosition(const Vector3& position, bool isLeft) {
	transform_.position = position;
	
	// isLeftパラメータに基づいて移動方向と左右判定を設定
	if (isLeft) {
		moveDirection_ = -1.0f; // 左に移動
		isLeftEnemy_ = true;     // 左の敵（1秒で死亡）
	} else {
		moveDirection_ = 1.0f;  // 右に移動
		isLeftEnemy_ = false;   // 右の敵（3秒で死亡）
	}
	
	if (modelResource_) {
		modelResource_->position_ = transform_.position;
	}
}

void SplitEnemy::Configure(const EnemySpawnParams& params) {
	// 基本的な Transform 設定
	transform_.position = params.position;
	transform_.rotation = params.rotation;
	transform_.scale = params.scale;

	// SplitEnemy 固有のパラメータ設定
	bool isLeft = params.GetBool("isLeft", false);
	float speed = params.GetFloat("moveSpeed", 2.0f);
	
	// 移動方向と左右判定を設定
	if (isLeft) {
		moveDirection_ = -1.0f; // 左に移動
		isLeftEnemy_ = true;     // 左の敵（1秒で死亡）
	} else {
		moveDirection_ = 1.0f;  // 右に移動
		isLeftEnemy_ = false;   // 右の敵（3秒で死亡）
	}
	
	moveSpeed_ = speed;

	// ModelResource に反映
	if (modelResource_) {
		modelResource_->position_ = transform_.position;
		modelResource_->rotate_ = transform_.rotation;
		modelResource_->scale_ = transform_.scale;
	}
}



