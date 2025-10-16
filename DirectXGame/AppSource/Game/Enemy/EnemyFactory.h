#pragma once
#include "BaseEnemy.h"
#include "EnemySpawnParams.h"
#include <functional>
#include <unordered_map>
#include <memory>
#include <string>

class ModelManager;
class Camera;

/// @brief 敵の生成を行うファクトリクラス
class EnemyFactory {
public:
	// 敵生成関数の型定義
	using EnemyCreator = std::function<std::unique_ptr<BaseEnemy>()>;
	
	/// @brief ファクトリを初期化
	/// @param modelManager モデルマネージャー
	/// @param camera カメラ
	void Initialize(ModelManager* modelManager, Camera* camera);
	
	/// @brief 敵の生成関数を登録
	/// @param enemyType 敵の種類名
	/// @param creator 生成関数
	void RegisterEnemy(const std::string& enemyType, EnemyCreator creator);
	
	/// @brief 敵を生成
	/// @param enemyType 敵の種類名
	/// @param params スポーンパラメータ
	/// @return 生成された敵（失敗時はnullptr）
	std::unique_ptr<BaseEnemy> CreateEnemy(const std::string& enemyType, const EnemySpawnParams& params);
	
	/// @brief 登録されている敵の種類一覧を取得
	/// @return 敵の種類名のリスト
	std::vector<std::string> GetRegisteredEnemyTypes() const;
	
private:
	ModelManager* modelManager_ = nullptr;
	Camera* camera_ = nullptr;
	std::unordered_map<std::string, EnemyCreator> enemyRegistry_;
	
	/// @brief 敵の基本設定を行う
	/// @param enemy 敵オブジェクト
	/// @param params スポーンパラメータ
	void ConfigureEnemyBase(BaseEnemy* enemy, const EnemySpawnParams& params);
};