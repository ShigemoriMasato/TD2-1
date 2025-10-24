#include "GameScene.h"
#include "../Player/Player.h"
#include "../Enemy/EnemyManager.h"
#include "../Enemy/EnemySpawnParams.h"
#include "../Collision/Collision.h"
#include "../Hook/Hook.h"
#include "../Item/Coin.h"

#include <Tools/FPS/FPSObserver.h>
#include <cmath>
#include <algorithm>


void GameScene::Initialize()
{
	camera_ = std::make_unique<CameraManager>();

	timeSlower_ = std::make_unique<TimeSlower>(fpsObserver_);

	//タイルマップ初期化
	{
		auto handle = modelManager_->LoadModel("testBlock");
		tileMap_ = std::make_unique<TileMap>(&physicsEngine_);
		levelLoader_.LoadLevel("Assets/Map/level1.json", *tileMap_);
		tileMap_->SetModelData(textureManager_, modelManager_->GetModelData(handle), camera_->GetCamera());
	}

	//プレイヤー初期化
	{
		auto player = std::make_unique<Player>(timeSlower_.get(), &physicsEngine_);
		auto handle = modelManager_->LoadModel("testBlock");
		physicsEngine_;
		player->Initialize(modelManager_->GetModelData(handle), camera_->GetCamera());
		player->SetKeyConfig(&keys_);
		player->SetTileMap(tileMap_.get());
		player_ = player.get();
		objects_.push_back(std::move(player));
	}

	//Cameraの初期化
	camera_->Initialize(&player_->GetTransform()->position);
	camera_->SetOffset({ 0.0f, 5.0f, -40.0f });

	{
		enemyManager_ = std::make_unique<EnemyManager>();
		enemyManager_->Initialize(modelManager_, camera_->GetCamera());

		levelLoader_.AddEnemy(*enemyManager_);
	}

	{
		//ワイヤー初期化
		//ワイヤーの描画をプレイヤーに任せているので、オブジェクトリストに追加されない
		wire_ = std::make_unique<Wire>();
		auto handle = modelManager_->LoadModel("testWire");
		wire_->Initialize(modelManager_->GetModelData(handle), camera_->GetCamera());
		player_->SetWire(wire_.get());

	}

	//hook
	{
		auto handle = modelManager_->LoadModel("testBlock");
		levelLoader_.AddGameObject<Hook>(objects_, modelManager_, handle, camera_->GetCamera());
		levelLoader_.AddGameObject<Coin>(objects_, modelManager_, handle, camera_->GetCamera(), &physicsEngine_, false);
	}

	{
		//ゴールテープの作成
		auto goalTape = std::make_unique<GoalTape>();
		int textureHandle = textureManager_->LoadTexture("Assets/Texture/goal.png");
		auto tileWorldSize = tileMap_->WorldSize();
		goalTape->Initialize(textureHandle, tileWorldSize.x, tileWorldSize.y, &physicsEngine_, camera_->GetCamera());
		goalTape_ = goalTape.get();
		objects_.push_back(std::move(goalTape));
	}

	{
		//PostEffect初期化
		postEffect_ = std::make_unique<PostEffectResource>();
		postEffect_->Initialize();
		postEffect_->SetJobs(PostEffectJob::Fade);
		postEffect_->input_ = OffScreenIndex::GameWindow;
		postEffect_->output_ = OffScreenIndex::SwapChain;

		// フェードインで開始（alpha = 1.0から0.0へ）
		postEffect_->data_.fade.alpha = 1.0f;
		postEffect_->data_.fade.type = FadeType::Dissolve;
		postEffect_->data_.fade.color = { 1.0f, 1.0f, 1.0f };  // 白に変更
		isFadingIn_ = true;
		fadeTimer_ = 0.0f;
	}

	{
		targetScope_ = std::make_unique<TargetScope>();
		targetScope_->Initialize(textureManager_->LoadTexture("Assets/Texture/scope.png"), player_, camera_->GetCamera());
	}

	{
		//ゴールイベント初期化
		goalEvent_ = std::make_unique<GoalEvent>(camera_.get());
	}

	render_->EndFrame(false);
}

std::unique_ptr<BaseScene> GameScene::Update()
{
	keys_ = commonData->keyManager_->GetKeyStates();

	timeSlower_->Update();

	float deltaTime = timeSlower_->GetDeltaTime();

	// フェードイン処理
	if (isFadingIn_)
	{
		fadeTimer_ += deltaTime;
		postEffect_->data_.fade.alpha = 1.0f - (fadeTimer_ / fadeDuration_);

		// フェードイン完了
		if (fadeTimer_ >= fadeDuration_){
			postEffect_->data_.fade.alpha = 0.0f;
			isFadingIn_ = false;
			// フェード完了後はジョブをクリアしてエフェクトをオフにする
			postEffect_->SetJobs(PostEffectJob::None);
		}

	} else {

		// フェード完了後もFadeジョブを設定（alpha=0.0で透明）
		postEffect_->data_.fade.alpha = 0.0f;
	}

	camera_->Update(deltaTime);
	camera_->DrawImGui();

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
	for (auto& object : objects_)
	{

		object->Update(deltaTime);
	}

	objects_.erase(std::remove_if(objects_.begin(), objects_.end(), [](auto& object)
		{
			return object->IsDead();
		}), objects_.end());

	//targetScope
	targetScope_->Update(deltaTime, commonData->keyManager_.get());


	physicsEngine_.Update(deltaTime);
	//オブジェクト間でのコリジョンチェック
	CheckAllCollision();

	if (keys_[Key::Debug])
	{
		return std::make_unique<GameScene>();
	}

	if (keys_[Key::DebugClear])
	{

	}

	goalEvent_->SetClear(player_->GetTransform()->position.x > goalX_);
	goalEvent_->Update(deltaTime);

	return nullptr;
}

void GameScene::Draw()
{
	render_->PreDraw(OffScreenIndex::GameWindow);

	for (auto& object : objects_)
	{
		object->Draw(render_);
	}

	tileMap_->Draw(render_);
	enemyManager_->Draw(render_);


	targetScope_->Draw(render_);

	render_->Draw(postEffect_.get());
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

		if ((selfA & maskB) && (selfB & maskA))continue;
		pair.first->OnCollision(pair.second);
		pair.second->OnCollision(pair.first);
	}

	// 敵とプレイヤーの当たり判定を手動でチェック
	if (enemyManager_ && player_)
	{
		auto enemies = enemyManager_->GetAllEnemyObjects();
		for (auto* enemy : enemies)
		{
			if (!enemy || !enemy->GetCollider()) continue;

			// プレイヤーのコライダーと敵のコライダーで判定
			if (Collision::CheckCollision(*player_->GetCollider(), *enemy->GetCollider()))
			{
				// マスクチェック
				auto* playerCollider = player_->GetCollider();
				auto* enemyCollider = enemy->GetCollider();

				auto selfPlayer = playerCollider->GetSelf();
				auto maskPlayer = playerCollider->GetMask();
				auto selfEnemy = enemyCollider->GetSelf();
				auto maskEnemy = enemyCollider->GetMask();

				if ((selfPlayer & maskEnemy) && (selfEnemy & maskPlayer))continue;
				player_->OnCollision(enemy);
				enemy->OnCollision(player_);

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
			if (object->GetCollider()->GetSelf() & (ColliderMask::ENEMY | ColliderMask::HOOK))
			{
				player_->AddTargets(object.get());
			}
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



