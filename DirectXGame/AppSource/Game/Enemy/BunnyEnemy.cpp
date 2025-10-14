#include "BunnyEnemy.h"
#include <Tools/FPS/FPSObserver.h>

BunnyEnemy::BunnyEnemy() {
}

void BunnyEnemy::Initialize(ModelData* modelData, Camera* camera) {
	// BaseObjectのInitializeを呼び出してモデルリソースを初期化
	BaseObject::Initialize(modelData, camera);

	// 敵の初期設定
	transform_.position = { 0.0f, 0.0f, 5.0f }; // 少し前方に配置
	transform_.rotation = { 0.0f, 0.0f, 0.0f };
	transform_.scale = { 1.0f, 1.0f, 1.0f };

	isActive_ = true;
}

void BunnyEnemy::Update() {
	if (!isActive_) return;

	// Y軸周りにデルタタイムを使用して回転させる
	transform_.rotation.y += rotationSpeed_ * FPSObserver::GetDeltatime();

	// ModelResourceのTransformを更新
	if (modelResource_) {
		modelResource_->SetMatrixData(transform_.scale, transform_.rotation, transform_.position);
	}
}

void BunnyEnemy::Draw(Render* render) {
	if (!isActive_) return;

	// BaseObjectのDrawを使用
	BaseObject::Draw(render);
}