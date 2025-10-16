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

	// 基本設定を適用
	ConfigureEnemyBase(enemy.get(), params);

	return enemy;
}

std::vector<std::string> EnemyFactory::GetRegisteredEnemyTypes() const {
	std::vector<std::string> types;
	for (const auto& pair : enemyRegistry_) {
		types.push_back(pair.first);
	}
	return types;
}

void EnemyFactory::ConfigureEnemyBase(BaseEnemy* enemy, const EnemySpawnParams& params) {
	// 位置設定（敵タイプによって個別処理）
	if (auto* divisionEnemy = dynamic_cast<DivisionEnemy*>(enemy)) {
		divisionEnemy->SetPosition(params.position);

		// DivisionEnemy固有のパラメータ設定
		bool canDivide = params.GetBool("canDivide", true);
		divisionEnemy->SetCanDivide(canDivide);

	} else if (auto* splitEnemy = dynamic_cast<SplitEnemy*>(enemy)) {
		bool isLeft = params.GetBool("isLeft", false);
		splitEnemy->SetPosition(params.position, isLeft);

	} else if (auto* trackerEnemy = dynamic_cast<TrackerEnemy*>(enemy)) {
		trackerEnemy->SetPosition(params.position);

		// TrackerEnemy固有のパラメータ設定
		float trackingSpeed = params.GetFloat("trackingSpeed", 1.0f);
		trackerEnemy->SetTrackingSpeed(trackingSpeed);

	} else {
		// その他の敵はTransformを直接設定
		enemy->GetTransform()->position = params.position;
	}

	// 共通パラメータの設定があれば追加
	// 例：HP、速度など
}