#include "GameScene.h"
#include "../Player/Player.h"

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

	//ワイヤー初期化
	//ワイヤーの描画をプレイヤーに任せているので、オブジェクトリストに追加されない
	{
		wire_ = std::make_unique<Wire>();
		auto handle = modelManager_->LoadModel("testWire");
		wire_->Initialize(modelManager_->GetModelData(handle), camera_.get());
		player_->SetWire(wire_.get());

		auto wire2 = std::make_unique<Wire>();
		wire2->Initialize(modelManager_->GetModelData(handle), camera_.get());
		objects_.push_back(std::move(wire2));
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
