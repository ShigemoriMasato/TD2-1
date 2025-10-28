#include "DashEnemy.h"
#include "../../Player/Player.h"
#include "../EnemySpawnParams.h"

void DashEnemy::Initialize(ModelData* modelData, Camera* camera) {
	BaseObject::Initialize(modelData, camera);

	// コライダーの設定
	collider_ = std::make_unique<AABBCollider>(
		ColliderTag::Dynamic,
		ColliderMask::ENEMY,
		ColliderMask::PLAYER
	);
	collider_->SetTransform(&transform_);
	collider_->SetSize(Vector3(1.0f, 1.0f, 1.0f));

	// 初期位置設定
	transform_.scale = Vector3(1.0f, 1.0f, 1.0f);

	isActive_ = true;
	isAlive_ = true;
}

void DashEnemy::Update(float deltaTime) {
	if (!isActive_ || !isAlive_) return;

	// クールダウンタイマーを更新
	if (cooldownTimer_ > 0.0f) {
		cooldownTimer_ -= deltaTime;
	}

	// モデルの位置を更新
	if (modelResource_) {
		modelResource_->position_ = transform_.position;
		
		// クールダウン中は色を変える
		if (cooldownTimer_ > 0.0f) {
			modelResource_->color_ = 0xff888888; // グレー
		} else {
			modelResource_->color_ = 0xffffffff; // 白
		}
	}
}

void DashEnemy::Draw(Render* render) {
	if (!isActive_ || !isAlive_) return;

	BaseObject::Draw(render);
}

void DashEnemy::OnCollision(BaseObject* other) {
	if (!other) return;

	// クールダウン中は処理しない
	if (cooldownTimer_ > 0.0f) return;

	// プレイヤーとの衝突時に前方方向へ飛ばす
	Player* player = dynamic_cast<Player*>(other);
	if (player) {
		// プレイヤーの現在の速度から移動方向を取得
		Vector2 currentVelocity = player->GetVelocity();
		Vector2 dashDirection;

		// 速度がほぼゼロの場合は、プレイヤーから敵への方向を使用
		if (currentVelocity.Length() < 0.1f) {
			// プレイヤーから敵への方向ベクトル
			Vector3 toEnemy = transform_.position - player->GetTransform()->position;
			// 敵からプレイヤーへの方向（反対方向）を使用
			dashDirection = Vector2(-toEnemy.x, -toEnemy.y);
		} else {
			// 現在の移動方向を使用
			dashDirection = currentVelocity;
		}

		// 正規化
		if (dashDirection.Length() > 0.0f) {
			dashDirection = dashDirection.Normalize();
		} else {
			// フォールバック：右方向
			dashDirection = Vector2(1.0f, 0.0f);
		}

		// 前方方向へ速度を加える
		Vector2 force = dashDirection * dashForce_;
		player->AddVelocity(force);

		// クールダウンを開始
		cooldownTimer_ = cooldownDuration_;

		// 視覚的フィードバック（色変更）
		if (modelResource_) {
			modelResource_->color_ = 0xffff00ff; // マゼンタ色に変更
		}
	}
}

void DashEnemy::Configure(const EnemySpawnParams& params) {
	// 基本パラメータの設定
	transform_.position = params.position;
	transform_.rotation = params.rotation;
	transform_.scale = params.scale;

	// カスタムパラメータから飛ばす力を取得
	dashForce_ = params.GetFloat("dashForce", 30.0f);
	
	// クールダウン時間をカスタマイズ可能に
	cooldownDuration_ = params.GetFloat("cooldownDuration", 0.5f);
}
