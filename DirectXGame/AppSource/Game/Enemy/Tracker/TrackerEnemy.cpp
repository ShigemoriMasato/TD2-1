#include "TrackerEnemy.h"
#include "../EnemySpawnParams.h"
#include <Tools/FPS/FPSObserver.h>
#include <Common/KeyConfig/KeyManager.h>

TrackerEnemy::TrackerEnemy() {
}

void TrackerEnemy::Initialize(ModelData* modelData, Camera* camera) {
	// BaseObjectのInitializeを呼び出してモデルリソースを初期化
	BaseObject::Initialize(modelData, camera);

	transform_.position = {0.0f, 0.0f, 0.0f};
	transform_.rotation = {0.0f, 0.0f, 0.0f};
	transform_.scale = {1.0f, 1.0f, 1.0f};

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

void TrackerEnemy::Update(float deltaTime) {
if (!isActive_ || !isAlive_) return;

	// テスト用：スペースキーでHP減少
	if (keys_[Key::Action]) {
		hp_--;
		if (hp_ <= 0) {
			Die();
			return;
		}
	}

	// プレイヤーに向かって移動
	Vector3 direction = playerPosition_ - transform_.position;
	float distance = direction.Length();
	
	if (distance > 0.1f) { // プレイヤーから離れている場合
		direction = direction / distance; // 正規化
		transform_.position += direction * trackingSpeed_ * deltaTime;
	}

	// 簡単なアニメーション：Y軸回転
	transform_.rotation.y += 2.0f * deltaTime;

	// ModelResourceのTransformを更新
	if (modelResource_) {
		modelResource_->scale_ = transform_.scale;
		modelResource_->rotate_ = transform_.rotation;
		modelResource_->position_ = transform_.position;
	}
}

void TrackerEnemy::Draw(Render* render) {
	if (!isActive_ || !isAlive_) return;
	BaseObject::Draw(render);
}

void TrackerEnemy::Die() {
	if (!isAlive_) return;

	isDead_ = true;
	isAlive_ = false;  // 生存フラグをfalseに
	// isActive_はそのまま（削除処理はEnemyManagerが行う）
	
	// 死亡エフェクトなどがあればここに追加
}

void TrackerEnemy::SetPosition(const Vector3& position) {
	transform_.position = position;
	if (modelResource_) {
		modelResource_->position_ = transform_.position;
	}
}

void TrackerEnemy::Configure(const EnemySpawnParams& params) {
	// 基本的な Transform 設定
	transform_.position = params.position;
	transform_.rotation = params.rotation;
	transform_.scale = params.scale;

	// TrackerEnemy 固有のパラメータ設定
	float speed = params.GetFloat("trackingSpeed", 1.0f);
	int hp = params.GetInt("hp", 3);
	
	SetTrackingSpeed(speed);
	hp_ = hp;

	// ModelResource に反映
	if (modelResource_) {
		modelResource_->position_ = transform_.position;
		modelResource_->rotate_ = transform_.rotation;
		modelResource_->scale_ = transform_.scale;
	}
}