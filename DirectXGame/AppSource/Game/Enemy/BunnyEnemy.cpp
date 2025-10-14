#include "BunnyEnemy.h"
#include <Tools/FPS/FPSObserver.h>

BunnyEnemy::BunnyEnemy() {
}

void BunnyEnemy::Initialize(ModelData* modelData, Camera* camera) {
	// BaseObjectのInitializeを呼び出してモデルリソースを初期化
	BaseObject::Initialize(modelData, camera);

	// 敵の初期設定
	transform_.position = { 5.0f, 0.0f, 3.0f }; // 少し前方に配置
	transform_.rotation = { 0.0f, 0.0f, 0.0f };
	transform_.scale = { 1.0f, 1.0f, 1.0f };

	isActive_ = true;
}

void BunnyEnemy::Update(float deltaTime) {
	if (!isActive_) return;

	// Y軸周りにデルタタイムを使用して回転させる
	transform_.rotation.y += rotationSpeed_ * deltaTime;

	// ModelResourceのTransformを更新
	if (modelResource_) {
		modelResource_->scale_ = transform_.scale;
		modelResource_->rotate_ = transform_.rotation;
		modelResource_->position_ = transform_.position;
		
	}
}

void BunnyEnemy::Draw(Render* render) {
	if (!isActive_) return;

	// BaseObjectのDrawを使用
	BaseObject::Draw(render);
}