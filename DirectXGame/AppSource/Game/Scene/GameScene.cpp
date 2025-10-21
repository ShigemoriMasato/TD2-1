#include "GameScene.h"
#include "../Player/Player.h"
#include "../Enemy/EnemyManager.h"
#include "../Enemy/EnemySpawnParams.h"
#include "../Collision/Collision.h"

#include <Tools/FPS/FPSObserver.h>
#include <cmath>
#include <algorithm>
#include "../Player/TestPlayer.h"

void GameScene::Initialize()
{
	camera_ = std::make_unique<DebugCamera>();
	camera_->Initialize();

	timeSlower_ = std::make_unique<TimeSlower>(fpsObserver_);

	//タイルマップ初期化
	{
		auto handle = modelManager_->LoadModel("testBlock");
		tileMap_ = std::make_unique<TileMap>(&physicsEngine_);
		levelLoader_.LoadLevel("Assets/Map/test.json", *tileMap_);
		tileMap_->SetModelData(textureManager_, modelManager_->GetModelData(handle), camera_.get());
	}


	//プレイヤー初期化
	{
		auto player = std::make_unique<Player>(timeSlower_.get(), &physicsEngine_);
		auto handle = modelManager_->LoadModel("testBlock");
		physicsEngine_;
		player->Initialize(modelManager_->GetModelData(handle), camera_.get());
		player->SetKeyConfig(&keys_);
		player->SetTileMap(tileMap_.get());
		player_ = player.get();
		objects_.push_back(std::move(player));
	}

	{
		enemyManager_ = std::make_unique<EnemyManager>();
		enemyManager_->Initialize(modelManager_, camera_.get());

		// プレイヤーの初期位置
		Vector3 playerPos = { 4.0f, 7.0f, 0.0f };

		//// TrackerEnemy（プレイヤーについてくる敵）を5体配置
		//// 敵1: プレイヤーの右側
		//enemyManager_->SpawnEnemy("TrackerEnemy", Vector3(playerPos.x + 5.0f, playerPos.y, 0.0f));

		//// 敵2: プレイヤーの左側
		//enemyManager_->SpawnEnemy("TrackerEnemy", Vector3(playerPos.x - 5.0f, playerPos.y, 0.0f));

		//// 敵3: プレイヤーの前方
		//enemyManager_->SpawnEnemy("TrackerEnemy", Vector3(playerPos.x, playerPos.y, 0.0f));

		//// 敵4: プレイヤーの後方
		//enemyManager_->SpawnEnemy("TrackerEnemy", Vector3(playerPos.x, playerPos.y, 0.0f));

		//// 敵5: プレイヤーの右前方（斜め）
		//enemyManager_->SpawnEnemy("TrackerEnemy", Vector3(playerPos.x + 4.0f, playerPos.y, 0.0f));

		enemyManager_->SpawnEnemy("DashEnemy", Vector3(4.0f, 1.0f, 0.0f));

	}

	{
		//ワイヤー初期化
		//ワイヤーの描画をプレイヤーに任せているので、オブジェクトリストに追加されない
		wire_ = std::make_unique<Wire>();
		auto handle = modelManager_->LoadModel("testWire");
		wire_->Initialize(modelManager_->GetModelData(handle), camera_.get());
		player_->SetWire(wire_.get());

	}

	{
		auto handle = modelManager_->LoadModel("testBlock");
		auto testPlayer = std::make_unique<TestPlayer>();
		testPlayer->Initialize(modelManager_->GetModelData(handle), camera_.get());
		testPlayer->SetKeyConfig(&keys_);
		testPlayer->SetActor(&physicsEngine_);
		objects_.push_back(std::move(testPlayer));
	}
}

std::unique_ptr<BaseScene> GameScene::Update()
{
	keys_ = commonData->keyManager_->GetKeyStates();
	camera_->Update();
	camera_->DrawImGui();

	timeSlower_->Update();

	float deltaTime = timeSlower_->GetDeltaTime();

	//ワイヤ出せる範囲をチェック
	CheckPlayerWireField();


	// EnemyManagerにキー入力を渡す
	if (enemyManager_)
	{
		enemyManager_->SetKeys(keys_);

		// プレイヤーの位置を敵に通知
		if (player_)
		{
			enemyManager_->SetPlayerPosition(player_->GetTransform()->position);
		}

		enemyManager_->Update(deltaTime);
	}

	//オブジェクト更新
	for (auto& object : objects_) {

		object->Update(deltaTime);
	}


	//オブジェクト間でのコリジョンチェック
	CheckAllCollision();
	physicsEngine_.Update(deltaTime);

	if (keys_[Key::Reverse])
	{
		return std::make_unique<GameScene>();
	}

	return nullptr;
}

void GameScene::Draw()
{
	render_->PreDraw();

	for (auto& object : objects_)
	{
		object->Draw(render_);
	}

	tileMap_->Draw(render_);
	enemyManager_->Draw(render_);
}

void GameScene::CheckAllCollision()
{
	const auto& collisionPairs = physicsEngine_.GetCollisionInfo();
	for (const auto& pair : collisionPairs)
	{
		auto* objAColider = pair.first->GetCollider();
		auto* objBColider = pair.second->GetCollider();

		auto selfA = objAColider->GetSelf();
		auto maskA = objBColider->GetMask();
		auto selfB = objBColider->GetSelf();
		auto maskB = objAColider->GetMask();

		if (!(selfA & maskB) || !(selfB & maskA))continue;
		pair.first->OnCollision(pair.second);
		pair.second->OnCollision(pair.first);
	}

	// 敵とプレイヤーの当たり判定を手動でチェック
	if (enemyManager_ && player_) {
		auto enemies = enemyManager_->GetAllEnemyObjects();
		for (auto* enemy : enemies) {
			if (!enemy || !enemy->GetCollider()) continue;

			// プレイヤーのコライダーと敵のコライダーで判定
			if (Collision::CheckCollision(*player_->GetCollider(), *enemy->GetCollider())) {
				// マスクチェック
				auto* playerCollider = player_->GetCollider();
				auto* enemyCollider = enemy->GetCollider();

				auto selfPlayer = playerCollider->GetSelf();
				auto maskPlayer = playerCollider->GetMask();
				auto selfEnemy = enemyCollider->GetSelf();
				auto maskEnemy = enemyCollider->GetMask();

				if ((selfPlayer & maskEnemy) && (selfEnemy & maskPlayer)) {
					player_->OnCollision(enemy);
					enemy->OnCollision(player_);
				}
			}
		}

	}
}

void GameScene::CheckPlayerWireField()
{
	for (auto& object : objects_)
	{
		if (object.get() == player_ || object.get() == wire_.get())continue;
		if (CollisionChecker(wire_.get(), object.get()))
		{
			player_->AddTargets(object.get());
		}
	}
	for (auto& enemy : enemyManager_->GetAllEnemyObjects())
	{
		if (CollisionChecker(wire_.get(), enemy))
		{
			player_->AddTargets(enemy);
		}
	}
}


