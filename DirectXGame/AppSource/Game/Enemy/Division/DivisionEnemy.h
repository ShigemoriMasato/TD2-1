#pragma once
#include "../BaseEnemy.h"
#include <functional>
#include <unordered_map>
#include <Common/KeyConfig/KeyManager.h>

// 分裂コールバック関数の型定義
using DivisionCallback = std::function<void(const Vector3& position, bool isLeft)>;

class DivisionEnemy :public BaseEnemy {
public:

	/// @brief　初期化
	/// @param modelData 
	/// @param camera 
	void Initialize(ModelData* modelData, Camera* camera) override;

	/// @brief 更新
	/// @param deltaTime 
	void Update(float deltaTime) override;

	/// @brief 描画
	/// @param render 
	void Draw(Render* render) override;

	/// @brief 分裂可能かどうかを設定
	/// @param canDivide 分裂可能フラグ
	void SetCanDivide(bool canDivide) { canDivide_ = canDivide; }

	/// @brief 分裂可能かどうかを取得
	/// @return 分裂可能フラグ
	bool CanDivide() const { return canDivide_; }

	/// @brief 分裂時のコールバック関数を設定
	/// @param callback コールバック関数
	void SetDivisionCallback(DivisionCallback callback) { divisionCallback_ = callback; }

	/// @brief テスト用：キー入力を設定
	/// @param keys キー入力状態
	void SetKeys(const std::unordered_map<Key, bool>& keys) { keys_ = keys; }

	/// @brief 共通インターフェース：死亡状態を取得
	/// @return 死亡フラグ
	bool IsDead() const override { return isDead_; }

	/// @brief 位置を設定
	/// @param position 位置
	void SetPosition(const Vector3& position);

	/// @brief 敵固有の設定を行う
	/// @param params スポーンパラメータ
	void Configure(const EnemySpawnParams& params) override;

private:
	// 分裂可能フラグ（trueなら死亡時に分裂、falseなら分裂しない）
	bool canDivide_ = true;

	// 死亡フラグ
	bool isDead_ = false;

	// 分裂時のコールバック関数
	DivisionCallback divisionCallback_ = nullptr;

	// 死亡処理
	void Die();
};

