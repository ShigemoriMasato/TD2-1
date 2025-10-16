#pragma once
#include "../BaseEnemy.h"
#include <unordered_map>
#include <Common/KeyConfig/KeyManager.h>

/// @brief 分裂後の敵クラス（DivisionEnemyから分裂して生成される）
class SplitEnemy : public BaseEnemy
{
public:
	SplitEnemy();
	~SplitEnemy() override = default;

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
	/// @param isLeft 左の敵かどうか（true: 左、false: 右）
	void SetPosition(const Vector3& position, bool isLeft);

	/// @brief テスト用：キー入力を設定
	/// @param keys キー入力状態
	void SetKeys(const std::unordered_map<Key, bool>& keys) { keys_ = keys; }

	/// @brief 共通インターフェース：死亡状態を取得
	/// @return 死亡フラグ
	bool IsDead() const override { return isDead_; }

	/// @brief 敵固有の設定を行う
	/// @param params スポーンパラメータ
	void Configure(const EnemySpawnParams& params) override;

private:
	// 移動速度
	float moveSpeed_ = 2.0f;
	
	// 移動方向（左右どちらに移動するか）
	float moveDirection_ = 1.0f; // 1.0f = 右, -1.0f = 左
	
	// 死亡フラグ
	bool isDead_ = false;

	// 生存時間（左の敵用）
	float lifeTime_ = 0.0f;
	
	// 左右判定フラグ
	bool isLeftEnemy_ = false; // true = 左の敵, false = 右の敵

	// 死亡処理
	void Die();
};
