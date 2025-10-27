#include "SelectScene.h"
#include "GameScene.h"
#include <Game/LevelLoader.h>
#include <algorithm>

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

	// ズームイン演出の初期化
	isZoomingIn_ = false;
	zoomInTimer_ = 0.0f;

	// ズーム後待機時間の初期化
	isWaitingAfterZoom_ = false;
	zoomWaitTimer_ = 0.0f;

	// ステージ選択の初期化（Level1が最初に選択される）
	selectedStageIndex_ = 0;
	previousStageIndex_ = 0;
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

	// ステージ1プレビューの初期化（最初は中央に選択状態で表示）
	{
		stagePreview1_ = std::make_unique<DrawResource>();
		stagePreview1_->Initialize(ShapeType::Plane);
		stagePreview1_->SetTextureHandle(offScreenManager_->GetOffScreenData(OffScreenIndex::Level1)->GetTextureGPUHandle());
		stagePreview1_->camera_ = uiCamera_.get();

		stagePreview1_->position_ = centerPosition_;
		stagePreview1_->scale_ = selectedScale_;
		stagePreview1_->rotate_ = { 0.0f, 0.0f, 0.0f };
		stagePreview1_->color_ = selectedColor_;

		// アニメーションデータの初期化
		stage1Animation_.currentPosition = centerPosition_;
		stage1Animation_.targetPosition = centerPosition_;
		stage1Animation_.currentScale = selectedScale_;
		stage1Animation_.targetScale = selectedScale_;
		stage1Animation_.currentRotation = { 0.0f, 0.0f, 0.0f };
		stage1Animation_.targetRotation = { 0.0f, 0.0f, 0.0f };
		stage1Animation_.currentColor = selectedColor_;
		stage1Animation_.targetColor = selectedColor_;
		stage1Animation_.animationTimer = 0.0f;
		stage1Animation_.isAnimating = false;
	}

	// ステージ2プレビューの初期化（右側に非選択状態で表示）
	{
		stagePreview2_ = std::make_unique<DrawResource>();
		stagePreview2_->Initialize(ShapeType::Plane);
		stagePreview2_->SetTextureHandle(offScreenManager_->GetOffScreenData(OffScreenIndex::Level2)->GetTextureGPUHandle());
		stagePreview2_->camera_ = uiCamera_.get();

		stagePreview2_->position_ = rightPosition_;
		stagePreview2_->scale_ = unselectedScale_;
		stagePreview2_->rotate_ = { 0.0f, 0.0f, 0.0f };
		stagePreview2_->color_ = unselectedColor_;

		// アニメーションデータの初期化
		stage2Animation_.currentPosition = rightPosition_;
		stage2Animation_.targetPosition = rightPosition_;
		stage2Animation_.currentScale = unselectedScale_;
		stage2Animation_.targetScale = unselectedScale_;
		stage2Animation_.currentRotation = { 0.0f, 0.0f, 0.0f };
		stage2Animation_.targetRotation = { 0.0f, 0.0f, 0.0f };
		stage2Animation_.currentColor = unselectedColor_;
		stage2Animation_.targetColor = unselectedColor_;
		stage2Animation_.animationTimer = 0.0f;
		stage2Animation_.isAnimating = false;
	}

	// ステージ3プレビューの初期化（最右側に非選択状態で表示、画面外）
	{
		stagePreview3_ = std::make_unique<DrawResource>();
		stagePreview3_->Initialize(ShapeType::Plane);
		stagePreview3_->SetTextureHandle(offScreenManager_->GetOffScreenData(OffScreenIndex::Level3)->GetTextureGPUHandle());
		stagePreview3_->camera_ = uiCamera_.get();

		stagePreview3_->position_ = farRightPosition_;
		stagePreview3_->scale_ = unselectedScale_;
		stagePreview3_->rotate_ = { 0.0f, 0.0f, 0.0f };
		stagePreview3_->color_ = unselectedColor_;

		// アニメーションデータの初期化
		stage3Animation_.currentPosition = farRightPosition_;
		stage3Animation_.targetPosition = farRightPosition_;
		stage3Animation_.currentScale = unselectedScale_;
		stage3Animation_.targetScale = unselectedScale_;
		stage3Animation_.currentRotation = { 0.0f, 0.0f, 0.0f };
		stage3Animation_.targetRotation = { 0.0f, 0.0f, 0.0f };
		stage3Animation_.currentColor = unselectedColor_;
		stage3Animation_.targetColor = unselectedColor_;
		stage3Animation_.animationTimer = 0.0f;
		stage3Animation_.isAnimating = false;
	}
}

std::unique_ptr<BaseScene> SelectScene::Update() {

	float deltaTime = fpsObserver_->GetDeltatime();

	// フェード処理の更新
	if (isFadingIn_) {
		UpdateFadeIn(deltaTime);
	} else if (isZoomingIn_) {
		UpdateZoomInEffect(deltaTime);
	} else if (isWaitingAfterZoom_) {
		UpdateZoomWait(deltaTime);
	} else if (isFadingOut_) {
		UpdateFadeOut(deltaTime);
	} else {
		// フェード中でない場合のみ入力を受け付ける
		ProcessInput();
	}

	// ステージプレビューのアニメーション更新
	UpdateStagePreviewAnimation(deltaTime);

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
	postEffect_->data_.gridTransition.pattern = 4.0f;    // 波紋状パターン
	postEffect_->SetJobs(PostEffectJob::GridTransition);
}

void SelectScene::UpdateZoomInEffect(float deltaTime) {

	// ズームインタイマーを進める
	zoomInTimer_ += deltaTime;
	float progress = std::min(zoomInTimer_ / zoomInDuration_, 1.0f);
	float easedProgress = EaseInCubic(progress);  // 加速しながらズーム

	// 選択されたステージを取得
	DrawResource* selectedStage = nullptr;
	StageAnimationData* selectedAnimation = nullptr;

	if (selectedStageIndex_ == 0) {
		selectedStage = stagePreview1_.get();
		selectedAnimation = &stage1Animation_;
	} else if (selectedStageIndex_ == 1) {
		selectedStage = stagePreview2_.get();
		selectedAnimation = &stage2Animation_;
	} else {
		selectedStage = stagePreview3_.get();
		selectedAnimation = &stage3Animation_;
	}

	if (selectedStage && selectedAnimation) {
		// スケールを画面いっぱいまで拡大
		Vector3 startScale = selectedAnimation->currentScale;
		selectedStage->scale_.x = startScale.x + (zoomScale_.x - startScale.x) * easedProgress;
		selectedStage->scale_.y = startScale.y + (zoomScale_.y - startScale.y) * easedProgress;

		// カラーを少し明るく（フラッシュ効果）
		float brightness = 1.0f + (0.5f * std::sin(progress * std::numbers::pi_v<float>));  // 最大1.5倍
		uint8_t r = static_cast<uint8_t>(std::min(255.0f, 255.0f * brightness));
		uint8_t g = static_cast<uint8_t>(std::min(255.0f, 255.0f * brightness));
		uint8_t b = static_cast<uint8_t>(std::min(255.0f, 255.0f * brightness));
		selectedStage->color_ = (r << 24) | (g << 16) | (b << 8) | 0xff;
	}

	// 他のステージを画面外に移動
	if (selectedStageIndex_ != 0) {
		stagePreview1_->position_.x = leftPosition_.x - (1400.0f * easedProgress);
	}
	if (selectedStageIndex_ != 1) {
		stagePreview2_->position_.x = (selectedStageIndex_ == 0 ? rightPosition_.x : leftPosition_.x) + 
			((selectedStageIndex_ == 0 ? 1400.0f : -1400.0f) * easedProgress);
	}
	if (selectedStageIndex_ != 2) {
		stagePreview3_->position_.x = rightPosition_.x + (1400.0f * easedProgress);
	}

	// ズームイン完了で待機時間開始
	if (zoomInTimer_ >= zoomInDuration_) {
		isZoomingIn_ = false;
		isWaitingAfterZoom_ = true;
		zoomWaitTimer_ = 0.0f;
	}
}

void SelectScene::UpdateZoomWait(float deltaTime) {
	
	// 待機タイマーを進める
	zoomWaitTimer_ += deltaTime;

	// 選択されたステージを取得してフラッシュ効果を継続
	DrawResource* selectedStage = nullptr;

	if (selectedStageIndex_ == 0) {
		selectedStage = stagePreview1_.get();
	} else if (selectedStageIndex_ == 1) {
		selectedStage = stagePreview2_.get();
	} else {
		selectedStage = stagePreview3_.get();
	}

	// 待機中はゆっくりと明るさを脈動させる
	if (selectedStage) {
		float pulseProgress = zoomWaitTimer_ / zoomWaitDuration_;
		float brightness = 1.0f + (0.2f * std::sin(pulseProgress * std::numbers::pi_v<float> * 4.0f));  // ゆっくり脈動
		uint8_t r = static_cast<uint8_t>(std::min(255.0f, 255.0f * brightness));
		uint8_t g = static_cast<uint8_t>(std::min(255.0f, 255.0f * brightness));
		uint8_t b = static_cast<uint8_t>(std::min(255.0f, 255.0f * brightness));
		selectedStage->color_ = (r << 24) | (g << 16) | (b << 8) | 0xff;
	}

	// 待機時間完了でフェードアウト開始
	if (zoomWaitTimer_ >= zoomWaitDuration_) {
		isWaitingAfterZoom_ = false;
		isFadingOut_ = true;
		fadeOutTimer_ = 0.0f;
	}
}

void SelectScene::ProcessInput() {

	// キー状態を取得
	auto keys = commonData->keyManager_->GetKeyStates();

	// ADキーで左右のステージを選択（キーが押された瞬間のみ反応）
	static bool prevKeyLeft = false;
	static bool prevKeyRight = false;

	if (keys[Key::Left] && !prevKeyLeft) {
		previousStageIndex_ = selectedStageIndex_;
		selectedStageIndex_ = (selectedStageIndex_ - 1 + (int)LevelIndex::kNumLevels) % (int)LevelIndex::kNumLevels;

		// 目標値を設定してアニメーション開始
		SetStagePreviewTargets();
	}
	if (keys[Key::Right] && !prevKeyRight) {
		previousStageIndex_ = selectedStageIndex_;
		selectedStageIndex_ = (selectedStageIndex_ + 1) % (int)LevelIndex::kNumLevels;

		// 目標値を設定してアニメーション開始
		SetStagePreviewTargets();
	}

	prevKeyLeft = keys[Key::Left];
	prevKeyRight = keys[Key::Right];

	// Spaceキーでステージ決定してズームイン演出開始
	if (keys[Key::Action]) {
		isStageSelected_ = true;
		StartZoomInEffect();
	}
}

void SelectScene::UpdateStagePreviewAnimation(float deltaTime) {

	// ズームイン中はステージ選択アニメーションをスキップ
	if (isZoomingIn_) return;

	// ステージ1のアニメーション更新
	if (stage1Animation_.isAnimating) {
		stage1Animation_.animationTimer += deltaTime;
		float t = std::min(stage1Animation_.animationTimer / stageAnimationDuration_, 1.0f);
		float easedT = EaseOutCubic(t);

		// 位置を補間
		stage1Animation_.currentPosition.x = stage1Animation_.currentPosition.x +
			(stage1Animation_.targetPosition.x - stage1Animation_.currentPosition.x) * easedT;
		stage1Animation_.currentPosition.y = stage1Animation_.currentPosition.y +
			(stage1Animation_.targetPosition.y - stage1Animation_.currentPosition.y) * easedT;

		// スケールを補間
		stage1Animation_.currentScale.x = stage1Animation_.currentScale.x +
			(stage1Animation_.targetScale.x - stage1Animation_.currentScale.x) * easedT;
		stage1Animation_.currentScale.y = stage1Animation_.currentScale.y +
			(stage1Animation_.targetScale.y - stage1Animation_.currentScale.y) * easedT;

		// カラーを補間（RGBAそれぞれ）
		uint8_t currentR = (stage1Animation_.currentColor >> 24) & 0xFF;
		uint8_t currentG = (stage1Animation_.currentColor >> 16) & 0xFF;
		uint8_t currentB = (stage1Animation_.currentColor >> 8) & 0xFF;
		uint8_t currentA = stage1Animation_.currentColor & 0xFF;

		uint8_t targetR = (stage1Animation_.targetColor >> 24) & 0xFF;
		uint8_t targetG = (stage1Animation_.targetColor >> 16) & 0xFF;
		uint8_t targetB = (stage1Animation_.targetColor >> 8) & 0xFF;
		uint8_t targetA = stage1Animation_.targetColor & 0xFF;

		uint8_t newR = static_cast<uint8_t>(currentR + (targetR - currentR) * easedT);
		uint8_t newG = static_cast<uint8_t>(currentG + (targetG - currentG) * easedT);
		uint8_t newB = static_cast<uint8_t>(currentB + (targetB - currentB) * easedT);
		uint8_t newA = static_cast<uint8_t>(currentA + (targetA - currentA) * easedT);

		stage1Animation_.currentColor = (newR << 24) | (newG << 16) | (newB << 8) | newA;

		// DrawResourceに適用
		stagePreview1_->position_ = stage1Animation_.currentPosition;
		stagePreview1_->scale_ = stage1Animation_.currentScale;
		stagePreview1_->color_ = stage1Animation_.currentColor;

		// アニメーション完了チェック
		if (t >= 1.0f) {
			stage1Animation_.isAnimating = false;
			stage1Animation_.animationTimer = 0.0f;
		}
	}

	// ステージ2のアニメーション更新
	if (stage2Animation_.isAnimating) {
		stage2Animation_.animationTimer += deltaTime;
		float t = std::min(stage2Animation_.animationTimer / stageAnimationDuration_, 1.0f);
		float easedT = EaseOutCubic(t);

		// 位置を補間
		stage2Animation_.currentPosition.x = stage2Animation_.currentPosition.x +
			(stage2Animation_.targetPosition.x - stage2Animation_.currentPosition.x) * easedT;
		stage2Animation_.currentPosition.y = stage2Animation_.currentPosition.y +
			(stage2Animation_.targetPosition.y - stage2Animation_.currentPosition.y) * easedT;

		// スケールを補間
		stage2Animation_.currentScale.x = stage2Animation_.currentScale.x +
			(stage2Animation_.targetScale.x - stage2Animation_.currentScale.x) * easedT;
		stage2Animation_.currentScale.y = stage2Animation_.currentScale.y +
			(stage2Animation_.targetScale.y - stage2Animation_.currentScale.y) * easedT;

		// カラーを補間
		uint8_t currentR = (stage2Animation_.currentColor >> 24) & 0xFF;
		uint8_t currentG = (stage2Animation_.currentColor >> 16) & 0xFF;
		uint8_t currentB = (stage2Animation_.currentColor >> 8) & 0xFF;
		uint8_t currentA = stage2Animation_.currentColor & 0xFF;

		uint8_t targetR = (stage2Animation_.targetColor >> 24) & 0xFF;
		uint8_t targetG = (stage2Animation_.targetColor >> 16) & 0xFF;
		uint8_t targetB = (stage2Animation_.targetColor >> 8) & 0xFF;
		uint8_t targetA = stage2Animation_.targetColor & 0xFF;

		uint8_t newR = static_cast<uint8_t>(currentR + (targetR - currentR) * easedT);
		uint8_t newG = static_cast<uint8_t>(currentG + (targetG - currentG) * easedT);
		uint8_t newB = static_cast<uint8_t>(currentB + (targetB - currentB) * easedT);
		uint8_t newA = static_cast<uint8_t>(currentA + (targetA - currentA) * easedT);

		stage2Animation_.currentColor = (newR << 24) | (newG << 16) | (newB << 8) | newA;

		// DrawResourceに適用
		stagePreview2_->position_ = stage2Animation_.currentPosition;
		stagePreview2_->scale_ = stage2Animation_.currentScale;
		stagePreview2_->color_ = stage2Animation_.currentColor;

		// アニメーション完了チェック
		if (t >= 1.0f) {
			stage2Animation_.isAnimating = false;
			stage2Animation_.animationTimer = 0.0f;
		}
	}

	// ステージ3のアニメーション更新
	if (stage3Animation_.isAnimating) {
		stage3Animation_.animationTimer += deltaTime;
		float t = std::min(stage3Animation_.animationTimer / stageAnimationDuration_, 1.0f);
		float easedT = EaseOutCubic(t);

		// 位置を補間
		stage3Animation_.currentPosition.x = stage3Animation_.currentPosition.x +
			(stage3Animation_.targetPosition.x - stage3Animation_.currentPosition.x) * easedT;
		stage3Animation_.currentPosition.y = stage3Animation_.currentPosition.y +
			(stage3Animation_.targetPosition.y - stage3Animation_.currentPosition.y) * easedT;

		// スケールを補間
		stage3Animation_.currentScale.x = stage3Animation_.currentScale.x +
			(stage3Animation_.targetScale.x - stage3Animation_.currentScale.x) * easedT;
		stage3Animation_.currentScale.y = stage3Animation_.currentScale.y +
			(stage3Animation_.targetScale.y - stage3Animation_.currentScale.y) * easedT;

		// カラーを補間
		uint8_t currentR = (stage3Animation_.currentColor >> 24) & 0xFF;
		uint8_t currentG = (stage3Animation_.currentColor >> 16) & 0xFF;
		uint8_t currentB = (stage3Animation_.currentColor >> 8) & 0xFF;
		uint8_t currentA = stage3Animation_.currentColor & 0xFF;

		uint8_t targetR = (stage3Animation_.targetColor >> 24) & 0xFF;
		uint8_t targetG = (stage3Animation_.targetColor >> 16) & 0xFF;
		uint8_t targetB = (stage3Animation_.targetColor >> 8) & 0xFF;
		uint8_t targetA = stage3Animation_.targetColor & 0xFF;

		uint8_t newR = static_cast<uint8_t>(currentR + (targetR - currentR) * easedT);
		uint8_t newG = static_cast<uint8_t>(currentG + (targetG - currentG) * easedT);
		uint8_t newB = static_cast<uint8_t>(currentB + (targetB - currentB) * easedT);
		uint8_t newA = static_cast<uint8_t>(currentA + (targetA - currentA) * easedT);

		stage3Animation_.currentColor = (newR << 24) | (newG << 16) | (newB << 8) | newA;

		// DrawResourceに適用
		stagePreview3_->position_ = stage3Animation_.currentPosition;
		stagePreview3_->scale_ = stage3Animation_.currentScale;
		stagePreview3_->color_ = stage3Animation_.currentColor;

		// アニメーション完了チェック
		if (t >= 1.0f) {
			stage3Animation_.isAnimating = false;
			stage3Animation_.animationTimer = 0.0f;
		}
	}
}

void SelectScene::SetStagePreviewTargets() {

	// 選択されたステージに応じて各ステージの目標位置、スケール、カラーを設定
	if (selectedStageIndex_ == 0) {
		// Level1が選択された状態
		// Stage1: 中央（選択）
		stage1Animation_.targetPosition = centerPosition_;
		stage1Animation_.targetScale = selectedScale_;
		stage1Animation_.targetColor = selectedColor_;

		// Stage2: 右側（非選択）
		stage2Animation_.targetPosition = rightPosition_;
		stage2Animation_.targetScale = unselectedScale_;
		stage2Animation_.targetColor = unselectedColor_;

		// Stage3: 最右側（画面外）
		stage3Animation_.targetPosition = farRightPosition_;
		stage3Animation_.targetScale = unselectedScale_;
		stage3Animation_.targetColor = unselectedColor_;
	} else if (selectedStageIndex_ == 1) {
		// Level2が選択された状態
		// Stage1: 左側（非選択）
		stage1Animation_.targetPosition = leftPosition_;
		stage1Animation_.targetScale = unselectedScale_;
		stage1Animation_.targetColor = unselectedColor_;

		// Stage2: 中央（選択）
		stage2Animation_.targetPosition = centerPosition_;
		stage2Animation_.targetScale = selectedScale_;
		stage2Animation_.targetColor = selectedColor_;

		// Stage3: 右側（非選択）
		stage3Animation_.targetPosition = rightPosition_;
		stage3Animation_.targetScale = unselectedScale_;
		stage3Animation_.targetColor = unselectedColor_;
	} else {
		// Level3が選択された状態
		// Stage1: 最左側（画面外）
		stage1Animation_.targetPosition = farLeftPosition_;
		stage1Animation_.targetScale = unselectedScale_;
		stage1Animation_.targetColor = unselectedColor_;

		// Stage2: 左側（非選択）
		stage2Animation_.targetPosition = leftPosition_;
		stage2Animation_.targetScale = unselectedScale_;
		stage2Animation_.targetColor = unselectedColor_;

		// Stage3: 中央（選択）
		stage3Animation_.targetPosition = centerPosition_;
		stage3Animation_.targetScale = selectedScale_;
		stage3Animation_.targetColor = selectedColor_;
	}

	// 全てのステージのアニメーションを開始
	stage1Animation_.isAnimating = true;
	stage1Animation_.animationTimer = 0.0f;

	stage2Animation_.isAnimating = true;
	stage2Animation_.animationTimer = 0.0f;

	stage3Animation_.isAnimating = true;
	stage3Animation_.animationTimer = 0.0f;
}

void SelectScene::StartZoomInEffect() {

	// ズームイン演出を開始
	isZoomingIn_ = true;
	zoomInTimer_ = 0.0f;

	// 選択されたステージのアニメーションデータを保存
	if (selectedStageIndex_ == 0) {
		stage1Animation_.currentScale = stagePreview1_->scale_;
		stage1Animation_.currentPosition = stagePreview1_->position_;
	} else if (selectedStageIndex_ == 1) {
		stage2Animation_.currentScale = stagePreview2_->scale_;
		stage2Animation_.currentPosition = stagePreview2_->position_;
	} else {
		stage3Animation_.currentScale = stagePreview3_->scale_;
		stage3Animation_.currentPosition = stagePreview3_->position_;
	}
}

float SelectScene::EaseOutCubic(float t) {
	return 1.0f - std::powf(1.0f - t, 3.0f);
}

float SelectScene::EaseInCubic(float t) {
	return t * t * t;
}

std::unique_ptr<BaseScene> SelectScene::CheckSceneTransition() {

	// フェードアウト完了でゲームシーンへ遷移
	if (isFadingOut_ && fadeOutTimer_ >= fadeOutDuration_) {
		// 選択されたステージのレベル名を取得
		auto levelMap = LevelLoader::GetLevelFileMap();
		std::string levelName;

		if (selectedStageIndex_ == (int)LevelIndex::Level0) {
			levelName = levelMap[LevelIndex::Level0];
			commonData->nextLevelIndex_ = LevelIndex::Level0;
		} else if (selectedStageIndex_ == (int)LevelIndex::Level1) {
			levelName = levelMap[LevelIndex::Level1];
			commonData->nextLevelIndex_ = LevelIndex::Level1;
		} else {
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

	// ステージ3プレビュー描画
	render_->Draw(stagePreview3_.get());

	render_->Draw(postEffect_.get());
}
