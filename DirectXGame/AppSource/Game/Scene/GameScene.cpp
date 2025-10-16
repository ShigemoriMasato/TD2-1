#include "GameScene.h"
#include "../Player/Player.h"
#include "../Enemy/EnemyManager.h"
#include "../Enemy/EnemySpawnParams.h"

#include <Tools/FPS/FPSObserver.h>
#include <cmath>
#include <algorithm>

void GameScene::Initialize()
{
	camera_ = std::make_unique<DebugCamera>();
	camera_->Initialize();

	timeSlower_ = std::make_unique<TimeSlower>(fpsObserver_);

	//プレイヤー初期化
	{
		auto player = std::make_unique<Player>(timeSlower_.get());
		auto handle = modelManager_->LoadModel("Cube");
		player->Initialize(modelManager_->GetModelData(handle), camera_.get());
		player->SetKeyConfig(&keys_);
		player_ = player.get();
		objects_.push_back(std::move(player));

	}

	{
		enemyManager_ = std::make_unique<EnemyManager>();
		enemyManager_->Initialize(modelManager_, camera_.get());
		
		//// 敵の配置を行う
		//SetupEnemies();
	}

	//ワイヤー初期化
	//ワイヤーの描画をプレイヤーに任せているので、オブジェクトリストに追加されない
	{
		wire_ = std::make_unique<Wire>();
		auto handle = modelManager_->LoadModel("testWire");
		wire_->Initialize(modelManager_->GetModelData(handle), camera_.get());
		player_->SetWire(wire_.get());

		auto wire2 = std::make_unique<Wire>();
		wire2->Initialize(modelManager_->GetModelData(handle), camera_.get());
		//objects_.push_back(std::move(wire2));
	}


	{
		auto handle = modelManager_->LoadModel("testBlock");
		tileMap_ = std::make_unique<TileMap>(&physicsEngine_);
		levelLoader_.LoadLevel("Assets/Map/test.json", *tileMap_);
		tileMap_->SetModelData(modelManager_->GetModelData(handle), camera_.get());
	}
}

std::unique_ptr<BaseScene> GameScene::Update()
{
	keys_ = commonData->keyManager_->GetKeyStates();
	camera_->Update();
	camera_->DrawImGui();

	timeSlower_->Update();

	float deltaTime = timeSlower_->GetDeltaTime();

	// EnemyManagerにキー入力を渡す
	if (enemyManager_) {
		enemyManager_->SetKeys(keys_);
		
		// プレイヤーの位置を敵に通知
		if (player_) {
			enemyManager_->SetPlayerPosition(player_->GetTransform()->position);
		}

		enemyManager_->Update(deltaTime);
	}

	//オブジェクト更新
	for (auto& object : objects_){
		object->Update(deltaTime);
	}

	//ワイヤ出せる範囲をチェック
	CheckPlayerWireField();
	//オブジェクト間でのコリジョンチェック
	CheckAllCollision();

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
		
		if(!(selfA & maskB) || !(selfB & maskA))continue;
		pair.first->OnCollision(pair.second);
        pair.second->OnCollision(pair.first);
	}
}

void GameScene::CheckPlayerWireField()
{
	std::list<BaseObject*> collisionObjects;

	for (auto& object : objects_)
	{
		if (object.get() == player_)continue;
		if (CollisionChecker(wire_.get(), object.get()))
		{
			collisionObjects.push_back(object.get());
		}
	}
	if (collisionObjects.size() > 0)
	{
		player_->SetTargets(&collisionObjects);
		isInWireField_ = true;
	}
	collisionObjects.clear();
}

//void GameScene::SetupEnemies() {
//	// 分裂可能な敵を配置（ボス敵）
//	{
//		EnemySpawnParams params;
//		params.position = { 5.0f, 0.0f, 0.0f };
//		params.rotation = { 0.0f, 0.0f, 0.0f };
//		params.scale = { 1.5f, 1.5f, 1.5f };  // 少し大きめに
//		params.modelName = "testEnemy";
//		params.teamTag = "boss";
//		params.customParams["canDivide"] = true;
//		
//		enemyManager_->SetupDivisionEnemy("DivisionEnemy", params);
//	}
//
//	// 追跡敵を複数配置
//	{
//		// 追跡敵1
//		EnemySpawnParams params1;
//		params1.position = { -3.0f, 0.0f, 2.0f };
//		params1.scale = { 0.8f, 0.8f, 0.8f };
//		params1.modelName = "testEnemy";
//		params1.teamTag = "normal";
//		params1.customParams["trackingSpeed"] = 1.2f;
//		params1.customParams["hp"] = 3;
//		
//		enemyManager_->SpawnEnemy("TrackerEnemy", params1);
//
//		// 追跡敵2
//		EnemySpawnParams params2;
//		params2.position = { -3.0f, 0.0f, -2.0f };
//		params2.scale = { 0.8f, 0.8f, 0.8f };
//		params2.modelName = "testEnemy";
//		params2.teamTag = "normal";
//		params2.customParams["trackingSpeed"] = 0.8f;
//		params2.customParams["hp"] = 5;
//		
//		enemyManager_->SpawnEnemy("TrackerEnemy", params2);
//
//		// 追跡敵3（少し遠くに配置）
//		EnemySpawnParams params3;
//		params3.position = { 8.0f, 0.0f, -5.0f };
//		params3.rotation = { 0.0f, 45.0f, 0.0f };  // 45度回転
//		params3.scale = { 1.0f, 1.0f, 1.0f };
//		params3.modelName = "testEnemy";
//		params3.teamTag = "elite";
//		params3.customParams["trackingSpeed"] = 2.0f;  // 高速
//		params3.customParams["hp"] = 8;
//		
//		enemyManager_->SpawnEnemy("TrackerEnemy", params3);
//	}
//
//	// 通常の敵をランダムな位置に配置
//	for (int i = 0; i < 3; ++i) {
//		EnemySpawnParams params;
//		params.position = { 
//			static_cast<float>(-5 + i * 3), 
//			0.0f, 
//			static_cast<float>(-8 + i * 2)
//		};
//		params.rotation = { 0.0f, static_cast<float>(i * 30), 0.0f };
//		params.modelName = "testEnemy";
//		params.teamTag = "normal";
//		params.customParams["trackingSpeed"] = 1.0f + i * 0.2f;
//		params.customParams["hp"] = 2 + i;
//		
//		enemyManager_->SpawnEnemy("TrackerEnemy", params);
//	}
//}
