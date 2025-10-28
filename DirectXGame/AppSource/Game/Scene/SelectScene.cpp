#include "SelectScene.h"
#include "GameScene.h"
#include <Game/LevelLoader.h>
#include <algorithm>
#include <Math/MyMath.h>

using namespace MyMath;

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

	// UI装飾タイマーの初期化
	decorationTimer_ = 0.0f;
	arrowPulseTimer_ = 0.0f;

	//テクスチャの読み込み
	{
		leftArrowReleaseTextureHandle_ = textureManager_->LoadTexture("Assets/Texture/SelectScene/ReleaseA.png");
		leftArrowTriggerTextureHandle_ = textureManager_->LoadTexture("Assets/Texture/SelectScene/TriggerA.png");
		rightArrowReleaseTextureHandle_ = textureManager_->LoadTexture("Assets/Texture/SelectScene/ReleaseD.png");
		rightArrowTriggerTextureHandle_ = textureManager_->LoadTexture("Assets/Texture/SelectScene/TriggerD.png");
	}

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


	// グラデーション背景
	{
		gradientBackground_ = std::make_unique<DrawResource>();
		gradientBackground_->Initialize(ShapeType::Plane);
		gradientBackground_->camera_ = uiCamera_.get();
		gradientBackground_->position_ = { 0.0f, 0.0f, 200.0f };
		gradientBackground_->scale_ = { 1280.0f, 720.0f, 1.0f };
		gradientBackground_->color_ = 0x0f1419ff;  // 濃い青黒色
	}

	// タイトルテキスト「STAGE SELECT」
	{
		titleText_ = std::make_unique<DrawResource>();
		titleText_->Initialize(ShapeType::Plane);
		titleText_->camera_ = uiCamera_.get();
		titleText_->position_ = { 0.0f, 280.0f, 40.0f };
		titleText_->scale_ = { 400.0f, 60.0f, 1.0f };
		titleText_->color_ = 0xd4dfe8ff;  // 淡い灰白色
		// TODO: テクスチャがあればここで設定
	}

	// 選択フレーム
	{
		selectionFrame_ = std::make_unique<DrawResource>();
		selectionFrame_->Initialize(ShapeType::Plane);
		selectionFrame_->camera_ = uiCamera_.get();
		selectionFrame_->position_ = centerPosition_;
		selectionFrame_->position_.z = 45.0f;  // ステージの後ろ
		selectionFrame_->scale_ = { 640.0f, 360.0f, 1.0f };
		selectionFrame_->color_ = 0x6a8a9a22;  // 半透明の青灰色
		selectionFrame_->psoConfig_.depthStencilID = DepthStencilID::Transparent;
	}

	// 左矢印
	{
		leftArrow_ = std::make_unique<DrawResource>();
		leftArrow_->Initialize(ShapeType::Plane);
		leftArrow_->camera_ = uiCamera_.get();
		leftArrow_->position_ = { -500.0f, 0.0f, 40.0f };
		leftArrow_->scale_ = { 128.0f, 128.0f, 1.0f };
		leftArrow_->color_ = 0xffffffff;
		// 初期状態はRelease（押されていない）テクスチャ
		leftArrow_->SetTextureHandle(leftArrowReleaseTextureHandle_);
	}

	// 右矢印
	{
		rightArrow_ = std::make_unique<DrawResource>();
		rightArrow_->Initialize(ShapeType::Plane);
		rightArrow_->camera_ = uiCamera_.get();
		rightArrow_->position_ = { 500.0f, 0.0f, 40.0f };
		rightArrow_->scale_ = { 128.0f, 128.0f, 1.0f };
		rightArrow_->color_ = 0xffffffff;
		// 初期状態はRelease（押されていない）テクスチャ
		rightArrow_->SetTextureHandle(rightArrowReleaseTextureHandle_);
	}

	// 操作説明テキスト
	{
		instructionText_ = std::make_unique<DrawResource>();
		instructionText_->Initialize(ShapeType::Plane);
		instructionText_->camera_ = uiCamera_.get();
		instructionText_->position_ = { 0.0f, -300.0f, 40.0f };
		instructionText_->scale_ = { 500.0f, 40.0f, 1.0f };
		instructionText_->color_ = 0x8a9aaaff;  // やや暗い灰色
		// TODO: 説明テクスチャがあればここで設定
	}

	// ステージ1プレビューの初期化
	{

		stagePreviews_.push_back(std::make_unique<DrawResource>());
		stagePreviews_.back()->Initialize(ShapeType::Plane);
		stagePreviews_.back()->SetTextureHandle(offScreenManager_->GetOffScreenData(OffScreenIndex::Level1)->GetTextureGPUHandle());
		stagePreviews_.back()->camera_ = uiCamera_.get();

		stagePreviews_.back()->position_ = centerPosition_;
		stagePreviews_.back()->scale_ = selectedScale_;
		stagePreviews_.back()->rotate_ = { 0.0f, 0.0f, 0.0f };
		stagePreviews_.back()->color_ = selectedColor_;



		// アニメーションデータの初期化
		stageAnimations_[0].currentPosition = centerPosition_;
		stageAnimations_[0].targetPosition = centerPosition_;
		stageAnimations_[0].currentScale = selectedScale_;
		stageAnimations_[0].targetScale = selectedScale_;
		stageAnimations_[0].currentRotation = { 0.0f, 0.0f, 0.0f };
		stageAnimations_[0].targetRotation = { 0.0f, 0.0f, 0.0f };
		stageAnimations_[0].currentColor = selectedColor_;
		stageAnimations_[0].targetColor = selectedColor_;
		stageAnimations_[0].animationTimer = 0.0f;
		stageAnimations_[0].isAnimating = false;

	}

	// ステージ2プレビューの初期化
	{

		stagePreviews_.push_back(std::make_unique<DrawResource>());
		stagePreviews_.back()->Initialize(ShapeType::Plane);
		stagePreviews_.back()->SetTextureHandle(offScreenManager_->GetOffScreenData(OffScreenIndex::Level2)->GetTextureGPUHandle());
		stagePreviews_.back()->camera_ = uiCamera_.get();
		stagePreviews_.back()->position_ = rightPosition_;
		stagePreviews_.back()->scale_ = unselectedScale_;
		stagePreviews_.back()->rotate_ = { 0.0f, 0.0f, 0.0f };
		stagePreviews_.back()->color_ = unselectedColor_;


		// アニメーションデータの初期化
		stageAnimations_[1].currentPosition = rightPosition_;
		stageAnimations_[1].targetPosition = rightPosition_;
		stageAnimations_[1].currentScale = unselectedScale_;
		stageAnimations_[1].targetScale = unselectedScale_;
		stageAnimations_[1].currentRotation = { 0.0f, 0.0f, 0.0f };
		stageAnimations_[1].targetRotation = { 0.0f, 0.0f, 0.0f };
		stageAnimations_[1].currentColor = unselectedColor_;
		stageAnimations_[1].targetColor = unselectedColor_;
		stageAnimations_[1].animationTimer = 0.0f;
		stageAnimations_[1].isAnimating = false;

	}

	// ステージ3プレビューの初期化
	{

		stagePreviews_.push_back(std::make_unique<DrawResource>());
		stagePreviews_.back()->Initialize(ShapeType::Plane);
		stagePreviews_.back()->SetTextureHandle(offScreenManager_->GetOffScreenData(OffScreenIndex::Level3)->GetTextureGPUHandle());
		stagePreviews_.back()->camera_ = uiCamera_.get();
		stagePreviews_.back()->position_ = farRightPosition_;
		stagePreviews_.back()->scale_ = unselectedScale_;
		stagePreviews_.back()->rotate_ = { 0.0f, 0.0f, 0.0f };
		stagePreviews_.back()->color_ = unselectedColor_;

		// アニメーションデータの初期化
		stageAnimations_[2].currentPosition = farRightPosition_;
		stageAnimations_[2].targetPosition = farRightPosition_;
		stageAnimations_[2].currentScale = unselectedScale_;
		stageAnimations_[2].targetScale = unselectedScale_;
		stageAnimations_[2].currentRotation = { 0.0f, 0.0f, 0.0f };
		stageAnimations_[2].targetRotation = { 0.0f, 0.0f, 0.0f };
		stageAnimations_[2].currentColor = unselectedColor_;
		stageAnimations_[2].targetColor = unselectedColor_;
		stageAnimations_[2].animationTimer = 0.0f;
		stageAnimations_[2].isAnimating = false;

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

	// UI装飾の更新
	if (!isZoomingIn_ && !isWaitingAfterZoom_ && !isFadingOut_) {
		UpdateUIDecorations(deltaTime);
	}

	// ステージプレビューのアニメーション更新
	UpdateStagePreviewAnimation(deltaTime);

	// シーン遷移判定
	return CheckSceneTransition();
}

void SelectScene::UpdateUIDecorations(float deltaTime) {

	decorationTimer_ += deltaTime;
	arrowPulseTimer_ += deltaTime;


	// 矢印の脈動
	float pulse = 1.0f + 0.2f * std::sin(arrowPulseTimer_ * 3.0f);
	leftArrow_->scale_ = { 128.0f * pulse, 128.0f * pulse, 1.0f };
	rightArrow_->scale_ = { 128.0f * pulse, 128.0f * pulse, 1.0f };

	// 選択フレームの脈動
	float framePulse = 1.0f + 0.05f * std::sin(decorationTimer_ * 2.0f);
	selectionFrame_->scale_ = {
		640.0f * framePulse,
		360.0f * framePulse,
		1.0f
	};

	// 選択フレームの位置を現在選択中のステージに合わせる
	selectionFrame_->position_ = centerPosition_;
	selectionFrame_->position_.z = 45.0f;
}

void SelectScene::UpdateFadeIn(float deltaTime) {

	// フェードインタイマーを進める
	fadeInTimer_ += deltaTime;
	float progress = std::min(fadeInTimer_ / fadeInDuration_, 1.0f);

	// progressを逆にして1.0→0.0にする
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

	// グリッドトランジションを0.0→1.0に進める
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
		selectedStage = stagePreviews_[0].get();
		selectedAnimation = &stageAnimations_[0];
	} else if (selectedStageIndex_ == 1) {
		selectedStage = stagePreviews_[1].get();
		selectedAnimation = &stageAnimations_[1];
	} else {
		selectedStage = stagePreviews_[2].get();
		selectedAnimation = &stageAnimations_[2];
	}

	if (selectedStage && selectedAnimation) {
		// スケールを拡大
		Vector3 startScale = selectedAnimation->currentScale;
		selectedStage->scale_.x = startScale.x + (zoomScale_.x - startScale.x) * easedProgress;
		selectedStage->scale_.y = startScale.y + (zoomScale_.y - startScale.y) * easedProgress;

		// カラーを少し明るく
		float brightness = 1.0f + (0.5f * std::sin(progress * std::numbers::pi_v<float>));  // 最大1.5倍
		uint8_t r = static_cast<uint8_t>(std::min(255.0f, 255.0f * brightness));
		uint8_t g = static_cast<uint8_t>(std::min(255.0f, 255.0f * brightness));
		uint8_t b = static_cast<uint8_t>(std::min(255.0f, 255.0f * brightness));
		selectedStage->color_ = (r << 24) | (g << 16) | (b << 8) | 0xff;
	}

	// 他のステージを画面外に移動
	if (selectedStageIndex_ != 0) {
		stagePreviews_[0]->position_.x = leftPosition_.x - (1400.0f * easedProgress);
	}
	if (selectedStageIndex_ != 1) {
		stagePreviews_[1]->position_.x = (selectedStageIndex_ == 0 ? rightPosition_.x : leftPosition_.x) +
			((selectedStageIndex_ == 0 ? 1400.0f : -1400.0f) * easedProgress);
	}
	if (selectedStageIndex_ != 2) {
		stagePreviews_[1]->position_.x = rightPosition_.x + (1400.0f * easedProgress);
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
		selectedStage = stagePreviews_[0].get();
	} else if (selectedStageIndex_ == 1) {
		selectedStage = stagePreviews_[1].get();
	} else {
		selectedStage = stagePreviews_[2].get();
	}

	// 待機中はゆっくりと明るさを脈動させる
	if (selectedStage) {
		float pulseProgress = zoomWaitTimer_ / zoomWaitDuration_;
		float brightness = 1.0f + (0.2f * std::sin(pulseProgress * std::numbers::pi_v<float> *4.0f));  // ゆっくり脈動
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

	// ADキーで左右のステージを選択
	static bool prevKeyLeft = false;
	static bool prevKeyRight = false;

	// 左キーが押された瞬間の処理
	if (keys[Key::Left] && !prevKeyLeft) {
		previousStageIndex_ = selectedStageIndex_;
		selectedStageIndex_ = (selectedStageIndex_ - 1 + (int)LevelIndex::kNumLevels) % (int)LevelIndex::kNumLevels;

		// 目標値を設定してアニメーション開始
		SetStagePreviewTargets();
	}
	
	// 右キーが押された瞬間の処理
	if (keys[Key::Right] && !prevKeyRight) {
		previousStageIndex_ = selectedStageIndex_;
		selectedStageIndex_ = (selectedStageIndex_ + 1) % (int)LevelIndex::kNumLevels;

		// 目標値を設定してアニメーション開始
		SetStagePreviewTargets();
	}

	// 左キーが押されている間はTriggerテクスチャに変更
	if (keys[Key::Left]) {
		leftArrow_->SetTextureHandle(leftArrowTriggerTextureHandle_);
	} else {
		leftArrow_->SetTextureHandle(leftArrowReleaseTextureHandle_);
	}

	// 右キーが押されている間はTriggerテクスチャに変更
	if (keys[Key::Right]) {
		rightArrow_->SetTextureHandle(rightArrowTriggerTextureHandle_);
	} else {
		rightArrow_->SetTextureHandle(rightArrowReleaseTextureHandle_);
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
	if (stageAnimations_[0].isAnimating) {
		stageAnimations_[0].animationTimer += deltaTime;
		float t = std::min(stageAnimations_[0].animationTimer / stageAnimationDuration_, 1.0f);
		float easedT = EaseOutCubic(t);

		// 位置を補間
		stageAnimations_[0].currentPosition.x = stageAnimations_[0].currentPosition.x +
			(stageAnimations_[0].targetPosition.x - stageAnimations_[0].currentPosition.x) * easedT;
		stageAnimations_[0].currentPosition.y = stageAnimations_[0].currentPosition.y +
			(stageAnimations_[0].targetPosition.y - stageAnimations_[0].currentPosition.y) * easedT;

		// スケールを補間
		stageAnimations_[0].currentScale.x = stageAnimations_[0].currentScale.x +
			(stageAnimations_[0].targetScale.x - stageAnimations_[0].currentScale.x) * easedT;
		stageAnimations_[0].currentScale.y = stageAnimations_[0].currentScale.y +
			(stageAnimations_[0].targetScale.y - stageAnimations_[0].currentScale.y) * easedT;

		// カラーを補間
		uint8_t currentR = (stageAnimations_[0].currentColor >> 24) & 0xFF;
		uint8_t currentG = (stageAnimations_[0].currentColor >> 16) & 0xFF;
		uint8_t currentB = (stageAnimations_[0].currentColor >> 8) & 0xFF;
		uint8_t currentA = stageAnimations_[0].currentColor & 0xFF;

		uint8_t targetR = (stageAnimations_[0].targetColor >> 24) & 0xFF;
		uint8_t targetG = (stageAnimations_[0].targetColor >> 16) & 0xFF;
		uint8_t targetB = (stageAnimations_[0].targetColor >> 8) & 0xFF;
		uint8_t targetA = stageAnimations_[0].targetColor & 0xFF;

		uint8_t newR = static_cast<uint8_t>(currentR + (targetR - currentR) * easedT);
		uint8_t newG = static_cast<uint8_t>(currentG + (targetG - currentG) * easedT);
		uint8_t newB = static_cast<uint8_t>(currentB + (targetB - currentB) * easedT);
		uint8_t newA = static_cast<uint8_t>(currentA + (targetA - currentA) * easedT);

		stageAnimations_[0].currentColor = (newR << 24) | (newG << 16) | (newB << 8) | newA;

		// DrawResourceに適用
		stagePreviews_[0]->position_ = stageAnimations_[0].currentPosition;
		stagePreviews_[0]->scale_ = stageAnimations_[0].currentScale;
		stagePreviews_[0]->color_ = stageAnimations_[0].currentColor;

		// アニメーション完了チェック
		if (t >= 1.0f) {
			stageAnimations_[0].isAnimating = false;
			stageAnimations_[0].animationTimer = 0.0f;
		}
	}

	// ステージ2のアニメーション更新
	if (stageAnimations_[1].isAnimating) {
		stageAnimations_[1].animationTimer += deltaTime;
		float t = std::min(stageAnimations_[1].animationTimer / stageAnimationDuration_, 1.0f);
		float easedT = EaseOutCubic(t);

		// 位置を補間
		stageAnimations_[1].currentPosition.x = stageAnimations_[1].currentPosition.x +
			(stageAnimations_[1].targetPosition.x - stageAnimations_[1].currentPosition.x) * easedT;
		stageAnimations_[1].currentPosition.y = stageAnimations_[1].currentPosition.y +
			(stageAnimations_[1].targetPosition.y - stageAnimations_[1].currentPosition.y) * easedT;

		// スケールを補間
		stageAnimations_[1].currentScale.x = stageAnimations_[1].currentScale.x +
			(stageAnimations_[1].targetScale.x - stageAnimations_[1].currentScale.x) * easedT;
		stageAnimations_[1].currentScale.y = stageAnimations_[1].currentScale.y +
			(stageAnimations_[1].targetScale.y - stageAnimations_[1].currentScale.y) * easedT;

		// カラーを補間
		uint8_t currentR = (stageAnimations_[1].currentColor >> 24) & 0xFF;
		uint8_t currentG = (stageAnimations_[1].currentColor >> 16) & 0xFF;
		uint8_t currentB = (stageAnimations_[1].currentColor >> 8) & 0xFF;
		uint8_t currentA = stageAnimations_[1].currentColor & 0xFF;

		uint8_t targetR = (stageAnimations_[1].targetColor >> 24) & 0xFF;
		uint8_t targetG = (stageAnimations_[1].targetColor >> 16) & 0xFF;
		uint8_t targetB = (stageAnimations_[1].targetColor >> 8) & 0xFF;
		uint8_t targetA = stageAnimations_[1].targetColor & 0xFF;

		uint8_t newR = static_cast<uint8_t>(currentR + (targetR - currentR) * easedT);
		uint8_t newG = static_cast<uint8_t>(currentG + (targetG - currentG) * easedT);
		uint8_t newB = static_cast<uint8_t>(currentB + (targetB - currentB) * easedT);
		uint8_t newA = static_cast<uint8_t>(currentA + (targetA - currentA) * easedT);

		stageAnimations_[1].currentColor = (newR << 24) | (newG << 16) | (newB << 8) | newA;

		// DrawResourceに適用
		stagePreviews_[1]->position_ = stageAnimations_[1].currentPosition;
		stagePreviews_[1]->scale_ = stageAnimations_[1].currentScale;
		stagePreviews_[1]->color_ = stageAnimations_[1].currentColor;

		// アニメーション完了チェック
		if (t >= 1.0f) {
			stageAnimations_[1].isAnimating = false;
			stageAnimations_[1].animationTimer = 0.0f;
		}
	}

	// ステージ3のアニメーション更新
	if (stageAnimations_[2].isAnimating) {
		stageAnimations_[2].animationTimer += deltaTime;
		float t = std::min(stageAnimations_[2].animationTimer / stageAnimationDuration_, 1.0f);
		float easedT = EaseOutCubic(t);

		// 位置を補間
		stageAnimations_[2].currentPosition.x = stageAnimations_[2].currentPosition.x +
			(stageAnimations_[2].targetPosition.x - stageAnimations_[2].currentPosition.x) * easedT;
		stageAnimations_[2].currentPosition.y = stageAnimations_[2].currentPosition.y +
			(stageAnimations_[2].targetPosition.y - stageAnimations_[2].currentPosition.y) * easedT;

		// スケールを補間
		stageAnimations_[2].currentScale.x = stageAnimations_[2].currentScale.x +
			(stageAnimations_[2].targetScale.x - stageAnimations_[2].currentScale.x) * easedT;
		stageAnimations_[2].currentScale.y = stageAnimations_[2].currentScale.y +
			(stageAnimations_[2].targetScale.y - stageAnimations_[2].currentScale.y) * easedT;

		// カラーを補間
		uint8_t currentR = (stageAnimations_[2].currentColor >> 24) & 0xFF;
		uint8_t currentG = (stageAnimations_[2].currentColor >> 16) & 0xFF;
		uint8_t currentB = (stageAnimations_[2].currentColor >> 8) & 0xFF;
		uint8_t currentA = stageAnimations_[2].currentColor & 0xFF;

		uint8_t targetR = (stageAnimations_[2].targetColor >> 24) & 0xFF;
		uint8_t targetG = (stageAnimations_[2].targetColor >> 16) & 0xFF;
		uint8_t targetB = (stageAnimations_[2].targetColor >> 8) & 0xFF;
		uint8_t targetA = stageAnimations_[2].targetColor & 0xFF;

		uint8_t newR = static_cast<uint8_t>(currentR + (targetR - currentR) * easedT);
		uint8_t newG = static_cast<uint8_t>(currentG + (targetG - currentG) * easedT);
		uint8_t newB = static_cast<uint8_t>(currentB + (targetB - currentB) * easedT);
		uint8_t newA = static_cast<uint8_t>(currentA + (targetA - currentA) * easedT);

		stageAnimations_[2].currentColor = (newR << 24) | (newG << 16) | (newB << 8) | newA;

		// DrawResourceに適用
		stagePreviews_[2]->position_ = stageAnimations_[2].currentPosition;
		stagePreviews_[2]->scale_ = stageAnimations_[2].currentScale;
		stagePreviews_[2]->color_ = stageAnimations_[2].currentColor;

		// アニメーション完了チェック
		if (t >= 1.0f) {
			stageAnimations_[2].isAnimating = false;
			stageAnimations_[2].animationTimer = 0.0f;
		}
	}
}

void SelectScene::SetStagePreviewTargets() {

	// 選択されたステージに応じて各ステージの目標位置、スケール、カラーを設定
	if (selectedStageIndex_ == 0) {
		// Level1が選択された状態
		// Stage1: 中央（選択）
		stageAnimations_[0].targetPosition = centerPosition_;
		stageAnimations_[0].targetScale = selectedScale_;
		stageAnimations_[0].targetColor = selectedColor_;

		// Stage2: 右側（非選択）
		stageAnimations_[1].targetPosition = rightPosition_;
		stageAnimations_[1].targetScale = unselectedScale_;
		stageAnimations_[1].targetColor = unselectedColor_;

		// Stage3: 最右側（画面外）
		stageAnimations_[2].targetPosition = farRightPosition_;
		stageAnimations_[2].targetScale = unselectedScale_;
		stageAnimations_[2].targetColor = unselectedColor_;
	} else if (selectedStageIndex_ == 1) {
		// Level2が選択された状態
		// Stage1: 左側（非選択）
		stageAnimations_[0].targetPosition = leftPosition_;
		stageAnimations_[0].targetScale = unselectedScale_;
		stageAnimations_[0].targetColor = unselectedColor_;

		// Stage2: 中央（選択）
		stageAnimations_[1].targetPosition = centerPosition_;
		stageAnimations_[1].targetScale = selectedScale_;
		stageAnimations_[1].targetColor = selectedColor_;

		// Stage3: 右側（非選択）
		stageAnimations_[2].targetPosition = rightPosition_;
		stageAnimations_[2].targetScale = unselectedScale_;
		stageAnimations_[2].targetColor = unselectedColor_;
	} else {
		// Level3が選択された状態
		// Stage1: 最左側（画面外）
		stageAnimations_[0].targetPosition = farLeftPosition_;
		stageAnimations_[0].targetScale = unselectedScale_;
		stageAnimations_[0].targetColor = unselectedColor_;

		// Stage2: 左側（非選択）
		stageAnimations_[1].targetPosition = leftPosition_;
		stageAnimations_[1].targetScale = unselectedScale_;
		stageAnimations_[1].targetColor = unselectedColor_;

		// Stage3: 中央（選択）
		stageAnimations_[2].targetPosition = centerPosition_;
		stageAnimations_[2].targetScale = selectedScale_;
		stageAnimations_[2].targetColor = selectedColor_;
	}

	// 全てのステージのアニメーションを開始
	stageAnimations_[0].isAnimating = true;
	stageAnimations_[0].animationTimer = 0.0f;

	stageAnimations_[1].isAnimating = true;
	stageAnimations_[1].animationTimer = 0.0f;

	stageAnimations_[2].isAnimating = true;
	stageAnimations_[2].animationTimer = 0.0f;
}

void SelectScene::StartZoomInEffect() {

	// ズームイン演出を開始
	isZoomingIn_ = true;
	zoomInTimer_ = 0.0f;

	// 選択されたステージのアニメーションデータを保存
	if (selectedStageIndex_ == 0) {
		stageAnimations_[0].currentScale = stagePreviews_[0]->scale_;
		stageAnimations_[0].currentPosition = stagePreviews_[0]->position_;
	} else if (selectedStageIndex_ == 1) {
		stageAnimations_[1].currentScale = stagePreviews_[1]->scale_;
		stageAnimations_[1].currentPosition = stagePreviews_[1]->position_;
	} else {
		stageAnimations_[2].currentScale = stagePreviews_[2]->scale_;
		stageAnimations_[2].currentPosition = stagePreviews_[2]->position_;
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

	// グラデーション背景（最背面）
	render_->Draw(gradientBackground_.get());


	// 選択フレーム（ステージの後ろ）
	if (!isZoomingIn_ && !isWaitingAfterZoom_) {
		render_->Draw(selectionFrame_.get());
	}

	// ステージプレビュー
	for (const auto& stagePreview : stagePreviews_) {
		render_->Draw(stagePreview.get());
	}


	//  UI要素（最前面）
	if (!isZoomingIn_ && !isWaitingAfterZoom_ && !isFadingOut_) {
		render_->Draw(titleText_.get());
		render_->Draw(leftArrow_.get());
		render_->Draw(rightArrow_.get());
		render_->Draw(instructionText_.get());
	}

	// 6. ポストエフェクト
	render_->Draw(postEffect_.get());
}
