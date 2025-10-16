#include "GameScene.h"
#include "../Player/Player.h"
#include "../Enemy/EnemyManager.h"

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

	enemyManager_->Update(deltaTime);

	}

	//オブジェクト更新
	for (auto& object : objects_){
		object->Update(deltaTime);
	}

	enemyManager_->Update(deltaTime);

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

	enemyManager_->Draw(render_);
}

void GameScene::CheckAllCollision()
{
	for (auto itA = objects_.begin(); itA != objects_.end(); itA++)
	{
		for (auto itB = std::next(itA); itB != objects_.end(); itB++)
		{
			if (CollisionChecker((*itA).get(), (*itB).get()))
			{
				(*itA)->OnCollision((*itB).get());
				(*itB)->OnCollision((*itA).get());
			}
		}
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
