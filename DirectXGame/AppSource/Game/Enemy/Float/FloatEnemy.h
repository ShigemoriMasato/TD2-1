#pragma once
#include "../BaseEnemy.h"

/// @brief プレイヤーを浮かばせる敵クラス
class FloatEnemy : public BaseEnemy {
public:
	/// @brief 初期化
	/// @param modelData モデルデータ
	/// @param camera カメラ
	void Initialize(ModelData* modelData, Camera* camera) override;

	/// @brief 更新
	/// @param deltaTime デルタタイム
	void Update(float deltaTime) override;

	/// @brief 描画
	/// @param render レンダー
	void Draw(Render* render) override;

	/// @brief 当たり判定時の処理
	/// @param other 衝突相手
	void OnCollision(BaseObject* other) override;

	/// @brief 敵固有の設定を行う
	/// @param params スポーンパラメータ
	void Configure(const EnemySpawnParams& params) override;

private:
	// プレイヤーを浮かせる上向きの力
	float floatForce_ = 150.0f;
};
