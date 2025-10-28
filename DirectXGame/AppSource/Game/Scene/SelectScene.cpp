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

	// ステージ選択の初期化（Level0が最初に選択される）
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

	// ステージプレビューの初期化
	// OffScreenIndexのマッピング: Level1, Level2, Level3, Level4, Level5
	const OffScreenIndex offScreenIndices[] = {
		OffScreenIndex::Level1,
		OffScreenIndex::Level2,
		OffScreenIndex::Level3,
		OffScreenIndex::Level4,
		OffScreenIndex::Level5
	};

	// 初期配置の位置テーブル
	const Vector3 initialPositions[] = {
		centerPosition_,      // Stage 0: 中央（選択状態）
		rightPosition_,       // Stage 1: 右側
		farRightPosition_,    // Stage 2: 右遠く
		farRightPosition_,    // Stage 4: 右遠く
		farRightPosition_,    // Stage 3: 右遠く
	};

	for (int i = 0; i < 5; ++i) {
		// DrawResourceの作成と初期化
		stagePreviews_.push_back(std::make_unique<DrawResource>());
		stagePreviews_.back()->Initialize(ShapeType::Plane);
		stagePreviews_.back()->SetTextureHandle(offScreenManager_->GetOffScreenData(offScreenIndices[i])->GetTextureGPUHandle());
		stagePreviews_.back()->camera_ = uiCamera_.get();
		stagePreviews_.back()->rotate_ = { 0.0f, 0.0f, 0.0f };

		// 最初のステージ（i == 0）は選択状態、それ以外は非選択状態
		bool isSelected = (i == 0);
		stagePreviews_.back()->position_ = initialPositions[i];
		stagePreviews_.back()->scale_ = isSelected ? selectedScale_ : unselectedScale_;
		stagePreviews_.back()->color_ = isSelected ? selectedColor_ : unselectedColor_;

		// アニメーションデータの初期化
		stageAnimations_[i].currentPosition = initialPositions[i];
		stageAnimations_[i].targetPosition = initialPositions[i];
		stageAnimations_[i].currentScale = isSelected ? selectedScale_ : unselectedScale_;
		stageAnimations_[i].targetScale = isSelected ? selectedScale_ : unselectedScale_;
		stageAnimations_[i].currentRotation = { 0.0f, 0.0f, 0.0f };
		stageAnimations_[i].targetRotation = { 0.0f, 0.0f, 0.0f };
		stageAnimations_[i].currentColor = isSelected ? selectedColor_ : unselectedColor_;
		stageAnimations_[i].targetColor = isSelected ? selectedColor_ : unselectedColor_;
		stageAnimations_[i].animationTimer = 0.0f;
		stageAnimations_[i].isAnimating = false;
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

	if (selectedStageIndex_ >= 0 && selectedStageIndex_ < 5) {
		selectedStage = stagePreviews_[selectedStageIndex_].get();
		selectedAnimation = &stageAnimations_[selectedStageIndex_];
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
	for (int i = 0; i < 5; ++i) {
		if (i != selectedStageIndex_) {
			float offset = (i < selectedStageIndex_) ? -1400.0f : 1400.0f;
			stagePreviews_[i]->position_.x += offset * easedProgress * deltaTime * 60.0f;
		}
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

	if (selectedStageIndex_ >= 0 && selectedStageIndex_ < 5) {
		selectedStage = stagePreviews_[selectedStageIndex_].get();
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

	// 全ステージのアニメーション更新（5個）
	for (int i = 0; i < 5; ++i) {
		if (stageAnimations_[i].isAnimating) {
			stageAnimations_[i].animationTimer += deltaTime;
			float t = std::min(stageAnimations_[i].animationTimer / stageAnimationDuration_, 1.0f);
			float easedT = EaseOutCubic(t);

			// 位置を補間
			stageAnimations_[i].currentPosition.x = stageAnimations_[i].currentPosition.x +
				(stageAnimations_[i].targetPosition.x - stageAnimations_[i].currentPosition.x) * easedT;
			stageAnimations_[i].currentPosition.y = stageAnimations_[i].currentPosition.y +
				(stageAnimations_[i].targetPosition.y - stageAnimations_[i].currentPosition.y) * easedT;

			// スケールを補間
			stageAnimations_[i].currentScale.x = stageAnimations_[i].currentScale.x +
				(stageAnimations_[i].targetScale.x - stageAnimations_[i].currentScale.x) * easedT;
			stageAnimations_[i].currentScale.y = stageAnimations_[i].currentScale.y +
				(stageAnimations_[i].targetScale.y - stageAnimations_[i].currentScale.y) * easedT;

			// カラーを補間
			uint8_t currentR = (stageAnimations_[i].currentColor >> 24) & 0xFF;
			uint8_t currentG = (stageAnimations_[i].currentColor >> 16) & 0xFF;
			uint8_t currentB = (stageAnimations_[i].currentColor >> 8) & 0xFF;
			uint8_t currentA = stageAnimations_[i].currentColor & 0xFF;

			uint8_t targetR = (stageAnimations_[i].targetColor >> 24) & 0xFF;
			uint8_t targetG = (stageAnimations_[i].targetColor >> 16) & 0xFF;
			uint8_t targetB = (stageAnimations_[i].targetColor >> 8) & 0xFF;
			uint8_t targetA = stageAnimations_[i].targetColor & 0xFF;

			uint8_t newR = static_cast<uint8_t>(currentR + (targetR - currentR) * easedT);
			uint8_t newG = static_cast<uint8_t>(currentG + (targetG - currentG) * easedT);
			uint8_t newB = static_cast<uint8_t>(currentB + (targetB - currentB) * easedT);
			uint8_t newA = static_cast<uint8_t>(currentA + (targetA - currentA) * easedT);

			stageAnimations_[i].currentColor = (newR << 24) | (newG << 16) | (newB << 8) | newA;

			// DrawResourceに適用
			stagePreviews_[i]->position_ = stageAnimations_[i].currentPosition;
			stagePreviews_[i]->scale_ = stageAnimations_[i].currentScale;
			stagePreviews_[i]->color_ = stageAnimations_[i].currentColor;

			// アニメーション完了チェック
			if (t >= 1.0f) {
				stageAnimations_[i].isAnimating = false;
				stageAnimations_[i].animationTimer = 0.0f;
			}
		}
	}
}

void SelectScene::SetStagePreviewTargets() {


	for (int i = 0; i < 5; ++i) {
		int relativePos = i - selectedStageIndex_;  // 選択ステージからの相対位置

		// 相対位置に応じて目標値を設定
		if (relativePos == 0) {
			// 選択中のステージ: 中央
			stageAnimations_[i].targetPosition = centerPosition_;
			stageAnimations_[i].targetScale = selectedScale_;
			stageAnimations_[i].targetColor = selectedColor_;
		} else if (relativePos == 1) {
			// 右隣のステージ
			stageAnimations_[i].targetPosition = rightPosition_;
			stageAnimations_[i].targetScale = unselectedScale_;
			stageAnimations_[i].targetColor = unselectedColor_;
		} else if (relativePos == -1) {
			// 左隣のステージ
			stageAnimations_[i].targetPosition = leftPosition_;
			stageAnimations_[i].targetScale = unselectedScale_;
			stageAnimations_[i].targetColor = unselectedColor_;
		} else if (relativePos == 2) {
			// 右2つ目のステージ
			stageAnimations_[i].targetPosition = farRightPosition_;
			stageAnimations_[i].targetScale = unselectedScale_;
			stageAnimations_[i].targetColor = unselectedColor_;
		} else if (relativePos == -2) {
			// 左2つ目のステージ
			stageAnimations_[i].targetPosition = farLeftPosition_;
			stageAnimations_[i].targetScale = unselectedScale_;
			stageAnimations_[i].targetColor = unselectedColor_;
		} else {
			// それ以外は画面外（さらに遠く）
			if (relativePos > 0) {
				stageAnimations_[i].targetPosition = { farRightPosition_.x + 700.0f * (relativePos - 2), 0.0f, 50.0f };
			} else {
				stageAnimations_[i].targetPosition = { farLeftPosition_.x - 700.0f * (-relativePos - 2), 0.0f, 50.0f };
			}
			stageAnimations_[i].targetScale = unselectedScale_;
			stageAnimations_[i].targetColor = unselectedColor_;
		}

		// アニメーション開始
		stageAnimations_[i].isAnimating = true;
		stageAnimations_[i].animationTimer = 0.0f;
	}
}

void SelectScene::StartZoomInEffect() {

	// ズームイン演出を開始
	isZoomingIn_ = true;
	zoomInTimer_ = 0.0f;

	// 選択されたステージのアニメーションデータを保存
	if (selectedStageIndex_ >= 0 && selectedStageIndex_ < 5) {
		stageAnimations_[selectedStageIndex_].currentScale = stagePreviews_[selectedStageIndex_]->scale_;
		stageAnimations_[selectedStageIndex_].currentPosition = stagePreviews_[selectedStageIndex_]->position_;
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

		// LevelIndexの配列を使用して選択されたステージに対応するインデックスを取得
		const LevelIndex levelIndices[] = {
			LevelIndex::Level0,
			LevelIndex::Level1,
			LevelIndex::Level2,
			LevelIndex::Level3,
			LevelIndex::Level4
		};

		// 範囲チェック
		if (selectedStageIndex_ >= 0 && selectedStageIndex_ < 5) {
			LevelIndex selectedLevel = levelIndices[selectedStageIndex_];
			commonData->nextLevelIndex_ = selectedLevel;

			// レベル名から".json"を削除
			std::string levelName = levelMap[selectedLevel];
			levelName = levelName.substr(0, levelName.find(".json"));
		}

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
