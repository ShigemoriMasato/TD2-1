#include "EnemyManager.h"
#include "Division/DivisionEnemy.h"
#include "Division/SplitEnemy.h"
#include <Resource/Model/ModelManager.h>
#include <algorithm>

EnemyManager::EnemyManager() {
	// BinaryManagerを初期化
	binaryManager_ = std::make_unique<BinaryManager>();
}

void EnemyManager::Initialize(ModelManager* modelManager, Camera* camera) {
	modelManager_ = modelManager;
	camera_ = camera;
	
	// Enemy Factory を初期化
	enemyFactory_.Initialize(modelManager, camera);
	
	// 初期敵配置はシーン側で行う（EnemyManagerは管理のみに専念）
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
	//分裂敵を生成
	EnemySpawnParams params;
	params.position = position;
	params.modelName = divisionModelName_;
	params.customParams["isLeft"] = isLeft;
	
	SpawnEnemy("SplitEnemy", params);
}

std::vector<BaseObject*> EnemyManager::GetAllEnemyObjects() {
	std::vector<BaseObject*> result;
	result.reserve(enemies_.size());
	
	for (auto& enemy : enemies_) {
		if (enemy && enemy->IsActive()) {
			result.push_back(enemy.get());
		}
	}
	
	return result;
}

// DivisionEnemyの分裂コールバック設定用ヘルパー関数
bool EnemyManager::SetupDivisionEnemy(const std::string& enemyType, const EnemySpawnParams& params) {
	if (SpawnEnemy(enemyType, params)) {
		// 追加待ちリストの最後に追加された敵（DivisionEnemy）にコールバックを設定
		if (!enemiesToAdd_.empty()) {
			if (auto* divisionEnemy = dynamic_cast<DivisionEnemy*>(enemiesToAdd_.back().get())) {
				divisionEnemy->SetDivisionCallback(
					[this](const Vector3& position, bool isLeft) {
						this->AddSplitEnemy(position, isLeft);
					}
				);
				return true;
			}
		}
	}
	return false;
}

// ========== バイナリファイルからの読み込み・保存機能の実装 ==========

void EnemyManager::SaveEnemyPlacement(const std::string& fileName) {
	// バージョン情報を登録
	binaryManager_->RegistOutput(float(1.0f), "version");
	
	// 敵の数を登録
	binaryManager_->RegistOutput(static_cast<int>(placementDataList_.size()), "enemyCount");
	
	// 各敵のデータを登録
	for (size_t i = 0; i < placementDataList_.size(); ++i) {
		const auto& data = placementDataList_[i];
		
		// 敵の種類
		binaryManager_->RegistOutput(data.enemyType, "enemyType");
		
		// 位置・回転・スケール
		binaryManager_->RegistOutput(data.position, "position");
		binaryManager_->RegistOutput(data.rotation, "rotation");
		binaryManager_->RegistOutput(data.scale, "scale");
		
		// モデル名・チームタグ
		binaryManager_->RegistOutput(data.modelName, "modelName");
		binaryManager_->RegistOutput(data.teamTag, "teamTag");
		binaryManager_->RegistOutput(data.priority, "priority");
		
		// カスタムパラメータ
		binaryManager_->RegistOutput(data.customFloat1, "customFloat1");
		binaryManager_->RegistOutput(data.customFloat2, "customFloat2");
		binaryManager_->RegistOutput(data.customInt1, "customInt1");
		binaryManager_->RegistOutput(data.customInt2, "customInt2");
		binaryManager_->RegistOutput(data.customBool1, "customBool1");
		binaryManager_->RegistOutput(data.customBool2, "customBool2");
		binaryManager_->RegistOutput(data.customString1, "customString1");
		binaryManager_->RegistOutput(data.customString2, "customString2");
	}
	
	// ファイルに書き出し
	binaryManager_->Write(fileName + ".bin");
}

bool EnemyManager::LoadEnemyPlacement(const std::string& fileName) {
	// ファイルから読み込み
	auto data = binaryManager_->Read(fileName + ".bin");
	
	if (data.empty()) {
		return false; // ファイルが見つからないか空
	}
	
	// バージョンチェック
	float version = BinaryManager::Reverse<float>(data[0]);
	if (version != 1.0f) {
		return false; // バージョンが一致しない
	}
	
	// 敵の数を取得
	int enemyCount = BinaryManager::Reverse<int>(data[1]);
	
	// データのインデックス
	size_t index = 2;
	
	// 各敵データを読み込んで生成
	for (int i = 0; i < enemyCount; ++i) {
		if (index + 15 > data.size()) {
			break; // データが不足している
		}
		
		EnemyPlacementData placementData;
		
		// データを取り出し
		placementData.enemyType = BinaryManager::Reverse<std::string>(data[index++]);
		placementData.position = BinaryManager::Reverse<Vector3>(data[index++]);
		placementData.rotation = BinaryManager::Reverse<Vector3>(data[index++]);
		placementData.scale = BinaryManager::Reverse<Vector3>(data[index++]);
		placementData.modelName = BinaryManager::Reverse<std::string>(data[index++]);
		placementData.teamTag = BinaryManager::Reverse<std::string>(data[index++]);
		placementData.priority = BinaryManager::Reverse<int>(data[index++]);
		placementData.customFloat1 = BinaryManager::Reverse<float>(data[index++]);
		placementData.customFloat2 = BinaryManager::Reverse<float>(data[index++]);
		placementData.customInt1 = BinaryManager::Reverse<int>(data[index++]);
		placementData.customInt2 = BinaryManager::Reverse<int>(data[index++]);
		placementData.customBool1 = BinaryManager::Reverse<bool>(data[index++]);
		placementData.customBool2 = BinaryManager::Reverse<bool>(data[index++]);
		placementData.customString1 = BinaryManager::Reverse<std::string>(data[index++]);
		placementData.customString2 = BinaryManager::Reverse<std::string>(data[index++]);
		
		// スポーンパラメータに変換
		EnemySpawnParams params = ConvertToSpawnParams(placementData);
		
		// 敵を生成
		if (placementData.enemyType == "DivisionEnemy") {
			SetupDivisionEnemy(placementData.enemyType, params);
		} else {
			SpawnEnemy(placementData.enemyType, params);
		}
	}
	
	return true;
}

void EnemyManager::RegisterEnemyPlacementData(const std::string& enemyType, const EnemySpawnParams& params) {
	// パラメータを配置データに変換して登録
	EnemyPlacementData data = ConvertToPlacementData(enemyType, params);
	placementDataList_.push_back(data);
}

void EnemyManager::ClearEnemyPlacementData() {
	placementDataList_.clear();
}

EnemyPlacementData EnemyManager::ConvertToPlacementData(const std::string& enemyType, const EnemySpawnParams& params) {
	EnemyPlacementData data;
	
	data.enemyType = enemyType;
	data.position = params.position;
	data.rotation = params.rotation;
	data.scale = params.scale;
	data.modelName = params.modelName;
	data.teamTag = params.teamTag;
	data.priority = params.priority;
	
	// カスタムパラメータを固定フィールドに変換（よく使う値のみ）
	// 使用例: speed, health, isLeft, direction など
	data.customFloat1 = params.GetFloat("speed", 0.0f);
	data.customFloat2 = params.GetFloat("health", 100.0f);
	data.customInt1 = params.GetInt("damage", 10);
	data.customInt2 = params.GetInt("scoreValue", 100);
	data.customBool1 = params.GetBool("isLeft", false);
	data.customBool2 = params.GetBool("canDivide", true);
	data.customString1 = params.GetString("aiType", "");
	data.customString2 = params.GetString("dropItem", "");
	
	return data;
}

EnemySpawnParams EnemyManager::ConvertToSpawnParams(const EnemyPlacementData& data) {
	EnemySpawnParams params;
	
	params.position = data.position;
	params.rotation = data.rotation;
	params.scale = data.scale;
	params.modelName = data.modelName;
	params.teamTag = data.teamTag;
	params.priority = data.priority;
	
	// 固定フィールドをカスタムパラメータに復元
	params.customParams["speed"] = data.customFloat1;
	params.customParams["health"] = data.customFloat2;
	params.customParams["damage"] = data.customInt1;
	params.customParams["scoreValue"] = data.customInt2;
	params.customParams["isLeft"] = data.customBool1;
	params.customParams["canDivide"] = data.customBool2;
	params.customParams["aiType"] = data.customString1;
	params.customParams["dropItem"] = data.customString2;
	
	return params;
}