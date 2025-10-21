#include "FloatEnemy.h"
#include "../../Player/Player.h"
#include "../EnemySpawnParams.h"

void FloatEnemy::Initialize(ModelData* modelData, Camera* camera) {
	BaseObject::Initialize(modelData, camera);

	// コライダーの設定
	collider_ = std::make_unique<AABBCollider>(
		ColliderTag::Static,
		ColliderMask::ENEMY,
		ColliderMask::PLAYER
	);
	collider_->SetTransform(&transform_);
	collider_->SetSize(Vector3(1.0f, 1.0f, 1.0f));

	// 初期位置設定（必要に応じて）
	transform_.scale = Vector3(1.0f, 1.0f, 1.0f);

	isActive_ = true;
	isAlive_ = true;
}

void FloatEnemy::Update(float deltaTime) {
	if (!isActive_ || !isAlive_) return;

	// モデルの位置を更新
	if (modelResource_) {
		modelResource_->position_ = transform_.position;
	}
}

void FloatEnemy::Draw(Render* render) {
	if (!isActive_ || !isAlive_) return;

	BaseObject::Draw(render);
}

void FloatEnemy::OnCollision(BaseObject* other) {
	if (!other) return;

	// プレイヤーとの衝突時に上向きの力を与える
	Player* player = dynamic_cast<Player*>(other);
	if (player) {
		// プレイヤーに上向きの速度を加える
		player->AddVelocity(Vector2(0.0f, floatForce_));

		// 視覚的フィードバック（色変更）
		if (modelResource_) {
			modelResource_->color_ = 0xff00ffff; // シアン色に変更
		}
	}
}

void FloatEnemy::Configure(const EnemySpawnParams& params) {
	// 基本パラメータの設定
	transform_.position = params.position;
	transform_.rotation = params.rotation;
	transform_.scale = params.scale;

	// カスタムパラメータから浮かせる力を取得
	floatForce_ = params.GetFloat("floatForce", 15.0f);
}
