#pragma once
#include "../BaseObject.h"
#include <Math/MyMath.h>
#include <unordered_map>
#include <Common/KeyConfig/KeyManager.h>


class BaseEnemy : public BaseObject
{
public:

	virtual ~BaseEnemy() = default;
	virtual void Initialize(ModelData* modelData, Camera* camera) = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Draw(Render* render) = 0;

	// プレイヤーの位置を設定（敵がプレイヤーの位置を知るため）
	virtual void SetPlayerPosition(const Vector3& playerPos) { playerPosition_ = playerPos; }

	// 敵がアクティブかどうか（処理対象かどうか）
	bool IsActive() const { return isActive_; }
	void SetActive(bool active) { isActive_ = active; }

	// 敵が生きているかどうか（ゲーム的な生死状態）
	bool IsAlive() const { return isAlive_; }
	void SetAlive(bool alive) { isAlive_ = alive; }

	// 共通インターフェース：キー入力を注入
	virtual void InjectInput(const std::unordered_map<Key, bool>& keys) { keys_ = keys; }

	// 共通インターフェース：死亡状態を取得
	virtual bool IsDead() const { return !isAlive_; }

protected:
	Vector3 playerPosition_ = { 0.0f, 0.0f, 0.0f };
	bool isActive_ = true;  // 処理対象フラグ（更新・描画するかどうか）
	bool isAlive_ = true;   // 生存フラグ（ゲーム的な生死状態）
	std::unordered_map<Key, bool> keys_;


};

