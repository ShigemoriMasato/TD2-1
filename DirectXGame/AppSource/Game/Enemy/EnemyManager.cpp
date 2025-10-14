#include "EnemyManager.h"
#include "BunnyEnemy.h"
#include "BunnyEnemy2.h"
#include "FloatingEnemy.h"
#include "../Engine/Resource/Model/ModelManager.h"

EnemyManager::EnemyManager() {
}


void EnemyManager::Initialize(ModelManager* modelManager, Camera* camera) {
	// 新しいInitialize：ModelManagerを使用して異なるモデルを設定
	modelManager_ = modelManager;
	camera_ = camera;
	
	// 各敵に異なるモデルを設定
	auto bunnyEnemy = std::make_unique<BunnyEnemy>();
	AddEnemy(std::move(bunnyEnemy), "testEnemy"); // BunnyEnemyにはtestEnemyモデル
	
	auto bunnyEnemy2 = std::make_unique<BunnyEnemy2>();
	AddEnemy(std::move(bunnyEnemy2), "Bunny"); // BunnyEnemy2にはBunnyモデル
	
	auto floatingEnemy = std::make_unique<FloatingEnemy>();
	AddEnemy(std::move(floatingEnemy), "testEnemy"); // FloatingEnemyにはtestEnemyモデル
}

void EnemyManager::Update() {
	// 全ての敵を更新
	for (auto& enemy : enemies_) {
		if (enemy && enemy->IsActive()) {
			enemy->Update();
		}
	}
}

void EnemyManager::Draw(Render* render) {
	// 全ての敵を描画
	for (auto& enemy : enemies_) {
		if (enemy && enemy->IsActive()) {
			enemy->Draw(render);
		}
	}
}


void EnemyManager::AddEnemy(std::unique_ptr<BaseEnemy> enemy, const std::string& modelName) {
	if (enemy && modelManager_ && camera_) {
		// ModelManagerからモデルデータを取得
		auto handle = modelManager_->LoadModel(modelName);
		ModelData* modelData = modelManager_->GetModelData(handle);
		
		// 敵にモデルデータとカメラを設定
		enemy->Initialize(modelData, camera_);
		
		// 敵リストに追加
		enemies_.push_back(std::move(enemy));
	}
}

void EnemyManager::AddEnemy(std::unique_ptr<BaseEnemy> enemy, ModelData* modelData) {
	if (enemy && modelData && camera_) {
		// 敵にモデルデータとカメラを設定
		enemy->Initialize(modelData, camera_);
		
		// 敵リストに追加
		enemies_.push_back(std::move(enemy));
	}
}

void EnemyManager::SetPlayerPosition(const Vector3& playerPos) {
	// 全ての敵にプレイヤーの位置を通知
	for (auto& enemy : enemies_) {
		if (enemy) {
			enemy->SetPlayerPosition(playerPos);
		}
	}
}


void EnemyManager::ClearEnemies() {
	enemies_.clear();
}
