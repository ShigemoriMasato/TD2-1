#include "EnemyFactory.h"
#include "Division/DivisionEnemy.h"
#include "Division/SplitEnemy.h"
#include "Tracker/TrackerEnemy.h"
#include <Resource/Model/ModelManager.h>

void EnemyFactory::Initialize(ModelManager* modelManager, Camera* camera) {
	modelManager_ = modelManager;
	camera_ = camera;

	// 標準の敵タイプを登録
	RegisterEnemy("DivisionEnemy", []() {
		return std::make_unique<DivisionEnemy>();
		});

	RegisterEnemy("SplitEnemy", []() {
		return std::make_unique<SplitEnemy>();
		});

	RegisterEnemy("TrackerEnemy", []() {
		return std::make_unique<TrackerEnemy>();
		});
}

void EnemyFactory::RegisterEnemy(const std::string& enemyType, EnemyCreator creator) {
	enemyRegistry_[enemyType] = creator;
}

std::unique_ptr<BaseEnemy> EnemyFactory::CreateEnemy(const std::string& enemyType, const EnemySpawnParams& params) {
	// 敵タイプがレジストリに登録されているかチェック
	auto it = enemyRegistry_.find(enemyType);
	if (it == enemyRegistry_.end()) {
		return nullptr; // 未登録の敵タイプ
	}

	// 敵を生成
	auto enemy = it->second();
	if (!enemy) {
		return nullptr; // 生成失敗
	}

	// モデルデータを取得して初期化
	auto handle = modelManager_->LoadModel(params.modelName);
	ModelData* modelData = modelManager_->GetModelData(handle);

	enemy->Initialize(modelData, camera_);

	// 敵固有の設定を適用（dynamic_cast不要！）
	enemy->Configure(params);

	return enemy;
}

std::vector<std::string> EnemyFactory::GetRegisteredEnemyTypes() const {
	std::vector<std::string> types;
	for (const auto& pair : enemyRegistry_) {
		types.push_back(pair.first);
	}
	return types;
}