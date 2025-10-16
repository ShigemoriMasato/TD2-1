#include "EnemyManager.h"
#include "Division/DivisionEnemy.h"
#include "Division/SplitEnemy.h"
#include <Resource/Model/ModelManager.h>
#include <algorithm>

EnemyManager::EnemyManager() {
}


void EnemyManager::Initialize(ModelManager* modelManager, Camera* camera) {
	modelManager_ = modelManager;
	camera_ = camera;
	
	// Enemy Factory を初期化
	enemyFactory_.Initialize(modelManager, camera);

	// 分裂可能な敵を1体スポーン（新しいAPI使用）
	EnemySpawnParams params;
	params.position = { 2.0f, 0.0f, 0.0f };
	params.modelName = "testEnemy";
	params.customParams["canDivide"] = true;
	
	if (SpawnEnemy("DivisionEnemy", params)) {
		// 追加待ちリストの最後に追加された敵（DivisionEnemy）にコールバックを設定
		if (!enemiesToAdd_.empty()) {
			if (auto* divisionEnemy = dynamic_cast<DivisionEnemy*>(enemiesToAdd_.back().get())) {
				divisionEnemy->SetDivisionCallback(
					[this](const Vector3& position, bool isLeft) {
						this->AddSplitEnemy(position, isLeft);
					}
				);
			}
		}
	}
}

void EnemyManager::Update(float deltaTime) {
	// 全ての敵にキー入力を設定（共通インターフェース使用）
	for (auto& enemy : enemies_) {
		if (enemy) {
			enemy->InjectInput(keys_);
		}
	}

	// 全ての敵を更新
	for (auto& enemy : enemies_) {
		if (enemy && enemy->IsActive()) {
			enemy->Update(deltaTime);
		}
	}

	// 死んだ敵を削除（共通インターフェース使用）
	enemies_.erase(
		std::remove_if(enemies_.begin(), enemies_.end(),
			[](const std::unique_ptr<BaseEnemy>& enemy) {
				if (enemy && enemy->IsDead()) {
					// 削除前にisActiveをfalseにして処理対象から除外
					enemy->SetActive(false);
					return true;
				}
				return false;
			}),
		enemies_.end()
	);

	// 追加待ちの敵を追加
	for (auto& enemy : enemiesToAdd_) {
		enemies_.push_back(std::move(enemy));
	}
	enemiesToAdd_.clear();
}

void EnemyManager::Draw(Render* render) {
	// 全ての敵を描画
	for (auto& enemy : enemies_) {
		if (enemy && enemy->IsActive()) {
			enemy->Draw(render);
		}
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

bool EnemyManager::SpawnEnemy(const std::string& enemyType, const EnemySpawnParams& params) {
	auto enemy = enemyFactory_.CreateEnemy(enemyType, params);
	if (!enemy) {
		return false; // 生成失敗
	}
	
	// 追加待ちリストに追加（Update中の追加を避けるため）
	enemiesToAdd_.push_back(std::move(enemy));
	return true;
}

bool EnemyManager::SpawnEnemy(const std::string& enemyType, const Vector3& position, const std::string& modelName) {
	EnemySpawnParams params;
	params.position = position;
	params.modelName = modelName;
	return SpawnEnemy(enemyType, params);
}

void EnemyManager::AddSplitEnemy(const Vector3& position, bool isLeft) {
	// Factory APIを使用して分裂敵を生成
	EnemySpawnParams params;
	params.position = position;
	params.modelName = divisionModelName_;
	params.customParams["isLeft"] = isLeft;
	
	SpawnEnemy("SplitEnemy", params);
}
