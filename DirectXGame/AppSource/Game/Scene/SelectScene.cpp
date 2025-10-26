#include "SelectScene.h"
#include "GameScene.h"
#include <Game/LevelLoader.h>

SelectScene::SelectScene() {
}

SelectScene::~SelectScene() {
}

void SelectScene::Initialize() {
	
	{
		// PostEffect初期化
		postEffect_ = std::make_unique<PostEffectResource>();
		postEffect_->Initialize();
		postEffect_->input_ = OffScreenIndex::Select;
		postEffect_->output_ = OffScreenIndex::SwapChain;
		
		// グリッドトランジション初期化（逆再生でフェードイン）
		postEffect_->data_.gridTransition.progress = 1.0f; // 1.0から始めて0.0に向かう
		postEffect_->data_.gridTransition.gridSize = 16.0f;
		postEffect_->data_.gridTransition.fadeColor = 0.0f;
		postEffect_->data_.gridTransition.pattern = 0.0f;
	}

	// フェードイン状態の初期化
	isFadingIn_ = true;
	fadeInTimer_ = 0.0f;

	// フェードアウト状態の初期化
	isFadingOut_ = false;
	fadeOutTimer_ = 0.0f;

	// ステージ選択の初期化
	selectedStageIndex_ = 0;
	isStageSelected_ = false;

	// UI用カメラの初期化（Orthographic投影）
	{
		uiCamera_ = std::make_unique<Camera>();
		OrthographicDesc desc{};
		desc.left = -640.0f;
		desc.top = 360.0f;
		desc.right = 640.0f;
		desc.bottom = -360.0f;
		desc.nearClip = 0.0f;
		desc.farClip = 1000.0f;
		uiCamera_->SetProjectionMatrix(desc);
		
		Transform cameraTransform{};
		cameraTransform.position = { 0.0f, 0.0f, 0.0f };
		cameraTransform.rotation = { 0.0f, 0.0f, 0.0f };
		cameraTransform.scale = { 1.0f, 1.0f, 1.0f };
		uiCamera_->SetTransform(cameraTransform);
		uiCamera_->MakeMatrix();
	}

	// ステージ1プレビューの初期化
	{
		stagePreview1_ = std::make_unique<DrawResource>();
		stagePreview1_->Initialize(ShapeType::Plane);
		// ステージ1のオフスクリーンテクスチャを設定（16:9の比率を維持）
		stagePreview1_->SetTextureHandle(offScreenManager_->GetOffScreenData(OffScreenIndex::Level1)->GetTextureGPUHandle());
		stagePreview1_->camera_ = uiCamera_.get();
		
		// 左側に配置、16:9の比率で表示（480x270のサイズ）
		stagePreview1_->position_ = { -300.0f, 0.0f, 50.0f };
		stagePreview1_->scale_ = { 480.0f, 270.0f, 1.0f };
		stagePreview1_->rotate_ = { 0.0f, 0.0f, 0.0f };
		stagePreview1_->color_ = 0xffffffff;
	}

	// ステージ2プレビューの初期化
	{
		stagePreview2_ = std::make_unique<DrawResource>();
		stagePreview2_->Initialize(ShapeType::Plane);
		// ステージ2のオフスクリーンテクスチャを設定（16:9の比率を維持）
		stagePreview2_->SetTextureHandle(offScreenManager_->GetOffScreenData(OffScreenIndex::Level2)->GetTextureGPUHandle());
		stagePreview2_->camera_ = uiCamera_.get();
		
		// 右側に配置、16:9の比率で表示（480x270のサイズ）
		stagePreview2_->position_ = { 300.0f, 0.0f, 50.0f };
		stagePreview2_->scale_ = { 480.0f, 270.0f, 1.0f };
		stagePreview2_->rotate_ = { 0.0f, 0.0f, 0.0f };
		stagePreview2_->color_ = 0xffffffff;
	}
}

std::unique_ptr<BaseScene> SelectScene::Update() {
	
	float deltaTime = fpsObserver_->GetDeltatime();

	// フェード処理の更新
	if (isFadingIn_) {
		UpdateFadeIn(deltaTime);
	}
	else if (isFadingOut_) {
		UpdateFadeOut(deltaTime);
	}
	else {
		// フェード中でない場合のみ入力を受け付ける
		ProcessInput();
	}

	// 選択されたステージの拡大表示
	if (!isFadingOut_) {
		// 選択されたステージを強調表示
		if (selectedStageIndex_ == 0) {
			stagePreview1_->scale_ = { 500.0f, 281.25f, 1.0f };  // 少し大きく
			stagePreview1_->color_ = 0xffffffff;  // 白
			stagePreview2_->scale_ = { 480.0f, 270.0f, 1.0f };  // 通常サイズ
			stagePreview2_->color_ = 0x808080ff;  // グレー（暗く）
		}
		else {
			stagePreview1_->scale_ = { 480.0f, 270.0f, 1.0f };  // 通常サイズ
			stagePreview1_->color_ = 0x808080ff;  // グレー（暗く）
			stagePreview2_->scale_ = { 500.0f, 281.25f, 1.0f };  // 少し大きく
			stagePreview2_->color_ = 0xffffffff;  // 白
		}
	}

	// シーン遷移判定
	return CheckSceneTransition();
}

void SelectScene::UpdateFadeIn(float deltaTime) {
	
	// フェードインタイマーを進める
	fadeInTimer_ += deltaTime;
	float progress = std::min(fadeInTimer_ / fadeInDuration_, 1.0f);
	
	// progressを逆にして1.0→0.0にする（グリッドが消えていく）
	postEffect_->data_.gridTransition.progress = 1.0f - progress;
	postEffect_->SetJobs(PostEffectJob::GridTransition);
	
	// フェードイン完了
	if (fadeInTimer_ >= fadeInDuration_) {
		isFadingIn_ = false;
		postEffect_->data_.gridTransition.progress = 0.0f;
		postEffect_->SetJobs(PostEffectJob::None);
	}
}

void SelectScene::UpdateFadeOut(float deltaTime) {
	
	// フェードアウトタイマーを進める
	fadeOutTimer_ += deltaTime;
	float progress = std::min(fadeOutTimer_ / fadeOutDuration_, 1.0f);
	
	// グリッドトランジションを0.0→1.0に進める（グリッドが広がっていく）
	postEffect_->data_.gridTransition.progress = progress;
	postEffect_->data_.gridTransition.gridSize = 16.0f;
	postEffect_->data_.gridTransition.fadeColor = 0.0f;  // 黒にフェード
	postEffect_->data_.gridTransition.pattern = 0.0f;    // 波紋状パターン
	postEffect_->SetJobs(PostEffectJob::GridTransition);
}

void SelectScene::ProcessInput() {
	
	// キー状態を取得
	auto keys = commonData->keyManager_->GetKeyStates();

	// ADキーで左右のステージを選択（キーが押された瞬間のみ反応）
	static bool prevKeyLeft = false;
	static bool prevKeyRight = false;

	if (keys[Key::Left] && !prevKeyLeft) {
		selectedStageIndex_ = (selectedStageIndex_ - 1 + 2) % 2;  // 0と1を循環
	}
	if (keys[Key::Right] && !prevKeyRight) {
		selectedStageIndex_ = (selectedStageIndex_ + 1) % 2;  // 0と1を循環
	}

	prevKeyLeft = keys[Key::Left];
	prevKeyRight = keys[Key::Right];

	// Spaceキーでステージ決定してゲームシーンへ遷移開始
	if (keys[Key::Action]) {
		isStageSelected_ = true;
		isFadingOut_ = true;
		fadeOutTimer_ = 0.0f;
	}
}

std::unique_ptr<BaseScene> SelectScene::CheckSceneTransition() {
	
	// フェードアウト完了でゲームシーンへ遷移
	if (isFadingOut_ && fadeOutTimer_ >= fadeOutDuration_) {
		// 選択されたステージのレベル名を取得
		auto levelMap = LevelLoader::GetLevelFileMap();
		std::string levelName;
		
		if (selectedStageIndex_ == 0) {
			levelName = levelMap[LevelIndex::Level1];
			commonData->nextLevelIndex_ = LevelIndex::Level1;
		}
		else {
			levelName = levelMap[LevelIndex::Level2];
			commonData->nextLevelIndex_ = LevelIndex::Level2;
		}

		// レベル名から".json"を削除
		levelName = levelName.substr(0, levelName.find(".json"));

		// 選択されたステージを読み込んでGameSceneを作成
		auto gameScene = std::make_unique<GameScene>();
		return gameScene;
	}

	return nullptr;
}

void SelectScene::Draw() {
	
	render_->PreDraw(OffScreenIndex::Select);
	
	// ステージ1プレビュー描画
	render_->Draw(stagePreview1_.get());
	
	// ステージ2プレビュー描画
	render_->Draw(stagePreview2_.get());

	render_->Draw(postEffect_.get());
}
