#include "BunnyEnemy2.h"
#include <Tools/FPS/FPSObserver.h>

BunnyEnemy2::BunnyEnemy2() {
}

void BunnyEnemy2::Initialize(ModelData* modelData, Camera* camera) {
	// BaseObjectのInitializeを呼び出してモデルリソースを初期化
	BaseObject::Initialize(modelData, camera);

	// 敵の初期設定（BunnyEnemyの隣に配置）
	transform_.position = { 0.0f, 0.0f, 0.0f }; // 右隣に配置
	transform_.rotation = { 0.0f, 0.0f, 0.0f };
	transform_.scale = { 1.0f, 1.0f, 1.0f };

	isActive_ = true;
}

void BunnyEnemy2::Update() {
	if (!isActive_) return;

	// Y軸周りに右回転させる（正の方向）
	transform_.rotation.y -= rotationSpeed_ * FPSObserver::GetDeltatime();

	// ModelResourceのTransformを更新
	if (modelResource_) {
		modelResource_->scale_ = transform_.scale;
		modelResource_->rotate_ = transform_.rotation;
		modelResource_->position_ = transform_.position;

	}
}

void BunnyEnemy2::Draw(Render* render) {
	if (!isActive_) return;

	// BaseObjectのDrawを使用
	BaseObject::Draw(render);
}