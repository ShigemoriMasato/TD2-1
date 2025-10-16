#pragma once
#include "BaseEnemy.h"
#include "EnemyFactory.h"
#include "EnemySpawnParams.h"
#include "../BaseObject.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <Common/KeyConfig/KeyManager.h>

class EnemyManager {
public:
	EnemyManager();
	~EnemyManager() = default;


	/// @brief 初期化
	/// @param modelManager 
	/// @param camera 
	void Initialize(ModelManager* modelManager, Camera* camera);

	/// @brief 更新
	/// @param deltaTime 
	void Update(float deltaTime);

	/// @brief 描画
	/// @param render 
	void Draw(Render* render);

	/// @brief 敵をスポーン（新しいFactory API）
	/// @param enemyType 敵の種類名
	/// @param params スポーンパラメータ
	/// @return 生成に成功したかどうか
	bool SpawnEnemy(const std::string& enemyType, const EnemySpawnParams& params);

	/// @brief 敵をスポーン（簡易版）
	/// @param enemyType 敵の種類名
	/// @param position 位置
	/// @param modelName モデル名
	/// @return 生成に成功したかどうか
	bool SpawnEnemy(const std::string& enemyType, const Vector3& position, const std::string& modelName = "testEnemy");

	/// @brief 敵にプレイヤーの位置を設定
	/// @param playerPos プレイヤーの位置
	void SetPlayerPosition(const Vector3& playerPos);

	/// @brief 全ての敵をクリア
	void ClearEnemies();

	/// @brief テスト用：キー入力を設定
	/// @param keys キー入力状態
	void SetKeys(const std::unordered_map<Key, bool>& keys) { keys_ = keys; }

	/// @brief Factoryへの参照を取得（カスタム敵の登録用）
	/// @return EnemyFactory参照
	EnemyFactory& GetFactory() { return enemyFactory_; }

	/// @brief DivisionEnemyをスポーンして分裂コールバックを自動設定
	/// @param enemyType 敵の種類名（通常は"DivisionEnemy"）
	/// @param params スポーンパラメータ
	/// @return 生成に成功したかどうか
	bool SetupDivisionEnemy(const std::string& enemyType, const EnemySpawnParams& params);

private:
	std::vector<std::unique_ptr<BaseEnemy>> enemies_;
	std::vector<std::unique_ptr<BaseEnemy>> enemiesToAdd_; // 追加待ちの敵
	ModelManager* modelManager_ = nullptr;
	Camera* camera_ = nullptr;
	std::unordered_map<Key, bool> keys_;

	// Enemy Factory
	EnemyFactory enemyFactory_;

	// 分裂用のモデル名（デフォルト）
	std::string divisionModelName_ = "testEnemy";

	/// @brief 分裂後の敵を追加（コールバック用）
	/// @param position 生成位置
	/// @param isLeft 左の敵かどうか
	void AddSplitEnemy(const Vector3& position, bool isLeft);
};

