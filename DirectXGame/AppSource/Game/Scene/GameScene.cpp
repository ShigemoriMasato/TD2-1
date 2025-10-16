#include "GameScene.h"
#include "../Player/Player.h"

#include <Tools/FPS/FPSObserver.h>
#include <cmath>

void GameScene::Initialize()
{
	camera_ = std::make_unique<DebugCamera>();
	camera_->Initialize();

	//プレイヤー初期化
	{
		auto player = std::make_unique<Player>();
		auto handle = modelManager_->LoadModel("Bunny");
		player->Initialize(modelManager_->GetModelData(handle), camera_.get());
		player->SetKeyConfig(&keys_);
		player_ = player.get();
		objects_.push_back(std::move(player));

	}

	{
		enemyManager_ = std::make_unique<EnemyManager>();
		enemyManager_->Initialize(modelManager_, camera_.get());
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

	auto deltaTime = FPSObserver::GetDeltatime() * slowmotionFactor_;
	//オブジェクト更新
	for (auto& object : objects_)
	{
		object->Update(deltaTime);
	}

	enemyManager_->Update(deltaTime);

	// テスト用：仮想プレイヤーの位置を左右に移動させる
	static float testPlayerX = -5.0f;
	static float direction = 1.0f;

	testPlayerX += direction * 2.0f * deltaTime; // 2.0f units per second

	// 範囲制限（-5.0f から 10.0f まで移動）
	if (testPlayerX > 10.0f)
	{
		testPlayerX = 10.0f;
		direction = -1.0f;
	}
	else if (testPlayerX < -5.0f)
	{
		testPlayerX = -5.0f;
		direction = 1.0f;
	}

	Vector3 testPlayerPos = { testPlayerX, 0.0f, 5.0f };

	// EnemyManagerにプレイヤー位置を通知
	for (auto& object : objects_)
	{
		// EnemyManagerの場合のみプレイヤー位置を設定
		if (auto* enemyManager = dynamic_cast<EnemyManager*>(object.get()))
		{
			enemyManager->SetPlayerPosition(testPlayerPos);
		}
	}

	//オブジェクト更新
	for (auto& object : objects_)
	{
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
