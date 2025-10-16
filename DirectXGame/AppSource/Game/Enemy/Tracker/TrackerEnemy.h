#pragma once
#include "../BaseEnemy.h"

/// @brief プレイヤーを追跡する敵クラス
class TrackerEnemy : public BaseEnemy
{
public:
	TrackerEnemy();
	~TrackerEnemy() override = default;

	/// @brief 初期化
	/// @param modelData モデルデータ
	/// @param camera カメラ
	void Initialize(ModelData* modelData, Camera* camera) override;

	/// @brief 更新
	/// @param deltaTime デルタタイム
	void Update(float deltaTime) override;

	/// @brief 描画
	/// @param render レンダラー
	void Draw(Render* render) override;

	/// @brief 位置を設定
	/// @param position 位置
	void SetPosition(const Vector3& position);

	/// @brief 共通インターフェース：死亡状態を取得
	/// @return 死亡フラグ
	bool IsDead() const override { return isDead_; }

	/// @brief 追跡速度を設定
	/// @param speed 追跡速度
	void SetTrackingSpeed(float speed) { trackingSpeed_ = speed; }

private:
	// 追跡速度
	float trackingSpeed_ = 1.0f;
	
	// 死亡フラグ
	bool isDead_ = false;
	
	// HP
	int hp_ = 3;

	// 死亡処理
	void Die();
};