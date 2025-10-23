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
#include <Tools/Binary/BinaryManager.h>

/// @brief 敵の配置データ（バイナリ保存用）
struct EnemyPlacementData {
	std::string enemyType;      // 敵の種類
	Vector3 position;    // 位置
	Vector3 rotation;           // 回転
	Vector3 scale;              // スケール
	std::string modelName;      // モデル名
	std::string teamTag; // チームタグ
	int priority;        // 優先度

	// customParamsの保存（簡易版：よく使う値のみ）
	float customFloat1;
	float customFloat2;
	int customInt1;
	int customInt2;
	bool customBool1;
	bool customBool2;
	std::string customString1;
	std::string customString2;

	EnemyPlacementData()
		: position{ 0.0f, 0.0f, 0.0f }
		, rotation{ 0.0f, 0.0f, 0.0f }
		, scale{ 1.0f, 1.0f, 1.0f }
		, modelName("testEnemy")
		, teamTag("enemy")
		, priority(0)
		, customFloat1(0.0f)
		, customFloat2(0.0f)
		, customInt1(0)
		, customInt2(0)
		, customBool1(false)
		, customBool2(false)
	{
	}
};

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

	/// @brief 全ての敵オブジェクトのリストを取得（物理エンジン登録用）
	/// @return 敵オブジェクトのリスト
	std::vector<BaseObject*> GetAllEnemyObjects();

	/// @brief DivisionEnemyをスポーンして分裂コールバックを自動設定
	/// @param enemyType 敵の種類名（通常は"DivisionEnemy"）
	/// @param params スポーンパラメータ
	/// @return 生成に成功したかどうか
	bool SetupDivisionEnemy(const std::string& enemyType, const EnemySpawnParams& params);

	// ========== バイナリファイルからの読み込み・保存機能 ==========

	/// @brief 敵配置データをバイナリファイルに保存
	/// @param fileName 保存するファイル名（拡張子なし）
	void SaveEnemyPlacement(const std::string& fileName);

	/// @brief 敵配置データをバイナリファイルから読み込んで敵を生成
	/// @param fileName 読み込むファイル名（拡張子なし）
	/// @return 読み込みに成功したかどうか
	bool LoadEnemyPlacement(const std::string& fileName);

	/// @brief 現在の敵配置データを登録（保存前に呼ぶ）
	/// @param enemyType 敵の種類
	/// @param params スポーンパラメータ
	void RegisterEnemyPlacementData(const std::string& enemyType, const EnemySpawnParams& params);

	/// @brief 登録済みの敵配置データをクリア
	void ClearEnemyPlacementData();

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

	// バイナリマネージャー（敵配置データの保存・読み込み用）
	std::unique_ptr<BinaryManager> binaryManager_;

	// 敵配置データのリスト（保存用）
	std::vector<EnemyPlacementData> placementDataList_;

	/// @brief 分裂後の敵を追加（コールバック用）
	/// @param position 生成位置
	/// @param isLeft 左の敵かどうか
	void AddSplitEnemy(const Vector3& position, bool isLeft);

	/// @brief EnemySpawnParamsからEnemyPlacementDataに変換
	/// @param enemyType 敵の種類
	/// @param params スポーンパラメータ
	/// @return 配置データ
	EnemyPlacementData ConvertToPlacementData(const std::string& enemyType, const EnemySpawnParams& params);

	/// @brief EnemyPlacementDataからEnemySpawnParamsに変換
	/// @param data 配置データ
	/// @return スポーンパラメータ
	EnemySpawnParams ConvertToSpawnParams(const EnemyPlacementData& data);
};

