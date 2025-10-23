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
	camera_ = std::make_unique<CameraManager>();

	timeSlower_ = std::make_unique<TimeSlower>(fpsObserver_);

	//タイルマップ初期化
	{
		auto handle = modelManager_->LoadModel("testBlock");
		tileMap_ = std::make_unique<TileMap>(&physicsEngine_);
		levelLoader_.LoadLevel("Assets/Map/test.json", *tileMap_);
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

	{
		enemyManager_ = std::make_unique<EnemyManager>();
		enemyManager_->Initialize(modelManager_, camera_->GetCamera());

		// ========== 選択1: バイナリデータ保存テスト（最初の実行時） ==========
		//// 各敵を一体ずつ配置してバイナリファイルに保存
		//
		//// プレイヤーの初期位置
		//Vector3 playerPos = { 4.0f, 7.0f, 0.0f };
		//
		//// 配置データをクリア
		//enemyManager_->ClearEnemyPlacementData();
		//
		//// 1. TrackerEnemy（追跡敵）を配置
		//{
		//	EnemySpawnParams params;
		//	params.position = { playerPos.x + 10.0f, playerPos.y, 0.0f };
		//	params.rotation = { 0.0f, 0.0f, 0.0f };
		//	params.scale = { 1.0f, 1.0f, 1.0f };
		//	params.modelName = "testEnemy";
		//	params.teamTag = "enemy";
		//	params.priority = 0;
		//	params.customParams["speed"] = 2.0f;
		//	params.customParams["health"] = 100.0f;
		//	params.customParams["damage"] = 10;
		//	params.customParams["scoreValue"] = 100;
		//	
		//	// 配置データに登録（保存用）
		//	enemyManager_->RegisterEnemyPlacementData("TrackerEnemy", params);
		//	
		//	// 実際に敵を生成
		//	enemyManager_->SpawnEnemy("TrackerEnemy", params);
		//}
		//
		//// 2. FloatEnemy（浮遊敵）を配置
		//{
		//	EnemySpawnParams params;
		//	params.position = { playerPos.x - 10.0f, playerPos.y + 5.0f, 0.0f };
		//	params.rotation = { 0.0f, 0.0f, 0.0f };
		//	params.scale = { 1.2f, 1.2f, 1.2f };
		//	params.modelName = "testEnemy";
		//	params.teamTag = "enemy";
		//	params.priority = 0;
		//	params.customParams["speed"] = 1.5f;
		//	params.customParams["health"] = 80.0f;
		//	params.customParams["damage"] = 15;
		//	params.customParams["scoreValue"] = 150;
		//	
		//	// 配置データに登録（保存用）
		//	enemyManager_->RegisterEnemyPlacementData("FloatEnemy", params);
		//	
		//	// 実際に敵を生成
		//	enemyManager_->SpawnEnemy("FloatEnemy", params);
		//}
		//
		//// 3. DivisionEnemy（分裂敵）を配置
		//{
		//	EnemySpawnParams params;
		//	params.position = { playerPos.x, playerPos.y, 15.0f };
		//	params.rotation = { 0.0f, 0.0f, 0.0f };
		//	params.scale = { 1.5f, 1.5f, 1.5f };
		//	params.modelName = "testEnemy";
		//	params.teamTag = "enemy";
		//	params.priority = 0;
		//	params.customParams["speed"] = 1.0f;
		//	params.customParams["health"] = 150.0f;
		//	params.customParams["damage"] = 20;
		//	params.customParams["scoreValue"] = 200;
		//	params.customParams["canDivide"] = true;
		//	
		//	// 配置データに登録（保存用）
		//	enemyManager_->RegisterEnemyPlacementData("DivisionEnemy", params);
		//	
		//	// 実際に敵を生成（分裂コールバック付き）
		//	enemyManager_->SetupDivisionEnemy("DivisionEnemy", params);
		//}
		//
		//// 4. DashEnemy（ダッシュ敵）を配置
		//{
		//	EnemySpawnParams params;
		//	params.position = { playerPos.x + 5.0f, playerPos.y - 3.0f, 0.0f };
		//	params.rotation = { 0.0f, 0.0f, 0.0f };
		//	params.scale = { 1.0f, 1.0f, 1.0f };
		//	params.modelName = "testEnemy";
		//	params.teamTag = "enemy";
		//	params.priority = 0;
		//	params.customParams["speed"] = 5.0f;
		//	params.customParams["health"] = 50.0f;
		//	params.customParams["damage"] = 25;
		//	params.customParams["scoreValue"] = 120;
		//	
		//	// 配置データに登録（保存用）
		//	enemyManager_->RegisterEnemyPlacementData("DashEnemy", params);
		//	
		//	// 実際に敵を生成
		//	enemyManager_->SpawnEnemy("DashEnemy", params);
		//}
		//
		//// バイナリファイルに保存（Assets/Binary/TestEnemyPlacement.bin として保存される）
		//enemyManager_->SaveEnemyPlacement("TestEnemyPlacement");
		
		// ========== 選択2: バイナリデータ読み込みテスト（2回目以降の実行時） ==========
		// 上記のコードをコメントアウトして、下記のコードを有効にすると
		// 保存したバイナリファイルから敵配置を読み込んで自動生成できます
		
		
		// 既存の敵をクリア
		enemyManager_->ClearEnemies();
		
		// バイナリファイルから敵配置を読み込んで自動生成
		bool loadSuccess = enemyManager_->LoadEnemyPlacement("TestEnemyPlacement");
		
		if (loadSuccess) {
			// 読み込み成功：敵が自動的に生成されている
			// TrackerEnemy, FloatEnemy, DivisionEnemy, DashEnemy が配置される
		} else {
			// 読み込み失敗：ファイルが見つからない等
			// デフォルトの敵配置にフォールバック
			enemyManager_->SpawnEnemy("DashEnemy", Vector3(4.0f, 1.0f, 0.0f));
		}
		
	}

	{
		//ワイヤー初期化
		//ワイヤーの描画をプレイヤーに任せているので、オブジェクトリストに追加されない
		wire_ = std::make_unique<Wire>();
		auto handle = modelManager_->LoadModel("testWire");
		wire_->Initialize(modelManager_->GetModelData(handle), camera_->GetCamera());
		player_->SetWire(wire_.get());

	}

	{

	  auto handle = modelManager_->LoadModel("testBlock");
	  auto testPlayer = std::make_unique<TestPlayer>();
	  testPlayer->Initialize(modelManager_->GetModelData(handle), camera_->GetCamera());
	  testPlayer->SetKeyConfig(&keys_);
	  testPlayer->SetActor(&physicsEngine_);
	  objects_.push_back(std::move(testPlayer));
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
		isFadingIn_ = true;
		fadeTimer_ = 0.0f;
	}
}

std::unique_ptr<BaseScene> GameScene::Update()
{
	keys_ = commonData->keyManager_->GetKeyStates();

	timeSlower_->Update();

	float deltaTime = timeSlower_->GetDeltaTime();

	// フェードイン処理
	if (isFadingIn_) {
		fadeTimer_ += deltaTime;
		postEffect_->data_.fade.alpha = 1.0f - (fadeTimer_ / fadeDuration_);
		
		// フェードイン完了
		if (fadeTimer_ >= fadeDuration_) {
			postEffect_->data_.fade.alpha = 0.0f;
			isFadingIn_ = false;
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
	render_->PreDraw(OffScreenIndex::GameWindow);

	for (auto& object : objects_)
	{
		object->Draw(render_);
	}

	tileMap_->Draw(render_);
	enemyManager_->Draw(render_);

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


