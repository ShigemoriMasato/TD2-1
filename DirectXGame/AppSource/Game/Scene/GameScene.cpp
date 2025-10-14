#include "GameScene.h"
#include "../Enemy/EnemyManager.h"
#include <Tools/FPS/FPSObserver.h>
#include <cmath>

void GameScene::Initialize()
{
	camera_ = std::make_unique<DebugCamera>();
	camera_->Initialize();

	
	{
		auto enemyManager = std::make_unique<EnemyManager>();
		enemyManager->Initialize(modelManager_, camera_.get());
		objects_.push_back(std::move(enemyManager));
	}

}

std::unique_ptr<BaseScene> GameScene::Update()
{
	keys_ = commonData->keyManager_->GetKeyStates();
	camera_->Update();

	// テスト用：仮想プレイヤーの位置を左右に移動させる
	static float testPlayerX = -5.0f;
	static float direction = 1.0f;
	float deltaTime = FPSObserver::GetDeltatime();
	
	testPlayerX += direction * 2.0f * deltaTime; // 2.0f units per second
	
	// 範囲制限（-5.0f から 10.0f まで移動）
	if (testPlayerX > 10.0f) {
		testPlayerX = 10.0f;
		direction = -1.0f;
	} else if (testPlayerX < -5.0f) {
		testPlayerX = -5.0f;
		direction = 1.0f;
	}
	
	Vector3 testPlayerPos = {testPlayerX, 0.0f, 5.0f};

	// EnemyManagerにプレイヤー位置を通知
	for (auto& object : objects_) {
		// EnemyManagerの場合のみプレイヤー位置を設定
		if (auto* enemyManager = dynamic_cast<EnemyManager*>(object.get())) {
			enemyManager->SetPlayerPosition(testPlayerPos);
		}
	}

	//オブジェクト更新
	for (auto& object : objects_)
	{
		object->Update();
	}

	return nullptr;
}

void GameScene::Draw()
{
	render_->PreDraw();



	// オブジェクトを描画
	for (auto& object : objects_)
	{
		object->Draw(render_);
	}
}
