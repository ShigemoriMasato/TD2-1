#include "SelectScene.h"
#include "GameScene.h"
#include <Game/LevelLoader.h>
#include <algorithm>
#include <Math/MyMath.h>
#include <cmath>

using namespace MyMath;

SelectScene::SelectScene() {
}

SelectScene::~SelectScene() {
}

void SelectScene::Initialize() {

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
		gradientBackground_->color_ = 0x0f1419ff;  // デフォルトカラー
	}

	// 背景パーティクルエフェクト初期化
	backgroundParticles_ = std::make_unique<BackgroundParticleEffect>();
	backgroundParticles_->Initialize(uiCamera_.get(), textureManager_);


	// トランジション初期化
	transition_ = std::make_unique<SelectSceneTransition>();
	transition_->Initialize();

	// UI初期化
	ui_ = std::make_unique<SelectSceneUI>();
	ui_->Initialize(uiCamera_.get(), textureManager_);

	// ステージカルーセル初期化
	stageCarousel_ = std::make_unique<StageCarousel>();
	const OffScreenIndex offScreenIndices[] = {
		OffScreenIndex::Level1,
		OffScreenIndex::Level2,
		OffScreenIndex::Level3,
		OffScreenIndex::Level4,
		OffScreenIndex::Level5
	};
	stageCarousel_->Initialize(uiCamera_.get(), offScreenIndices, offScreenManager_);

	// 入力ハンドラー初期化
	inputHandler_ = std::make_unique<SelectSceneInputHandler>();
	inputHandler_->Initialize(audio_);

	// ステージ選択変更時のコールバック設定
	inputHandler_->SetOnStageChangeCallback([this](int direction) -> bool {
		int currentIndex = stageCarousel_->GetSelectedStageIndex();
		int newIndex = currentIndex + direction;

		// 両端でストップ（ラップアラウンドしない）
		if (newIndex < 0 || newIndex >= (int)LevelIndex::kNumLevels) {
			return false;  // 範囲外の場合は移動失敗を返す
		}

		stageCarousel_->SetSelectedStageIndex(newIndex);

		// 背景グリッドアニメーションをトリガー
		transition_->TriggerBackgroundGrid();

		return true;  // 移動成功
		});

	// ステージ決定時のコールバック設定
	inputHandler_->SetOnStageConfirmCallback([this]() {
		transition_->StartZoomIn();
		});

	// 矢印テクスチャ更新のコールバック設定
	inputHandler_->SetOnArrowUpdateCallback([this](bool isLeftPressed, bool isRightPressed) {
		ui_->UpdateArrowTextures(isLeftPressed, isRightPressed);
		});

	// BGMの再生
	int bgmSoundHandle = audio_->Load("BGM/Normal.mp3");

	if (commonData->bgmPlayHandle_ / 10 != bgmSoundHandle) {
		audio_->Stop(commonData->bgmPlayHandle_);
		commonData->bgmPlayHandle_ = audio_->Play(bgmSoundHandle, true);
	}
}

std::unique_ptr<BaseScene> SelectScene::Update() {

	float deltaTime = fpsObserver_->GetDeltatime();

	// 背景パーティクルの更新（常に更新）
	backgroundParticles_->Update(deltaTime);



	// トランジション処理の更新
	if (transition_->IsFadingIn()) {
		transition_->UpdateFadeIn(deltaTime);
	} else if (transition_->IsZoomingIn()) {
		transition_->UpdateZoomIn(deltaTime, stageCarousel_.get(), stageCarousel_->GetSelectedStageIndex());
	} else if (transition_->IsWaitingAfterZoom()) {
		if (transition_->UpdateZoomWait(deltaTime, stageCarousel_.get(), stageCarousel_->GetSelectedStageIndex())) {
			// 待機完了でフェードアウト開始
			transition_->StartFadeOut();
		}
	} else if (transition_->IsFadingOut()) {
		transition_->UpdateFadeOut(deltaTime);
	} else {
		// トランジション中でない場合のみ入力を受け付ける
		auto keys = commonData->keyManager_->GetKeyStates();
		inputHandler_->ProcessInput(keys);
	}

	// 背景グリッドアニメーションの更新（常に実行）
	transition_->UpdateBackgroundGrid(deltaTime);

	// UI装飾の更新
	if (!transition_->IsZoomingIn() && !transition_->IsWaitingAfterZoom() && !transition_->IsFadingOut()) {
		Vector3 centerPos = { 0.0f, 0.0f, 50.0f };
		ui_->Update(deltaTime, centerPos);

		// 矢印の表示状態を更新（両端のステージでは対応する矢印を非表示）
		int currentIndex = stageCarousel_->GetSelectedStageIndex();
		bool canMoveLeft = (currentIndex > 0);
		bool canMoveRight = (currentIndex < (int)LevelIndex::kNumLevels - 1);
		ui_->UpdateArrowVisibility(canMoveLeft, canMoveRight);
	}


	// ステージプレビューのアニメーション更新（ズームイン中とズーム待機中はスキップ）
	bool skipAnimation = transition_->IsZoomingIn() || transition_->IsWaitingAfterZoom() || transition_->IsFadingOut();
	if (!skipAnimation) {
		stageCarousel_->Update(deltaTime, false);
	}

	// シーン遷移判定
	return CheckSceneTransition();
}

std::unique_ptr<BaseScene> SelectScene::CheckSceneTransition() {

	// フェードアウト完了でゲームシーンへ遷移
	if (transition_->IsFadeOutComplete()) {
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
		int selectedStageIndex = stageCarousel_->GetSelectedStageIndex();
		if (selectedStageIndex >= 0 && selectedStageIndex < 5) {
			LevelIndex selectedLevel = levelIndices[selectedStageIndex];
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

	// 背景パーティクル（背景の上）
	backgroundParticles_->Draw(render_);

	// 選択フレーム（ステージの後ろ）
	if (!transition_->IsZoomingIn() && !transition_->IsWaitingAfterZoom()) {
		render_->Draw(ui_->GetSelectionFrame());
	}

	// ステージプレビュー
	for (const auto& stagePreview : stageCarousel_->GetStagePreviews()) {
		render_->Draw(stagePreview.get());
	}

	// UI要素（最前面）
	if (!transition_->IsZoomingIn() && !transition_->IsWaitingAfterZoom() && !transition_->IsFadingOut()) {
		render_->Draw(ui_->GetTitleText());
		render_->Draw(ui_->GetLeftArrow());
		render_->Draw(ui_->GetRightArrow());
		render_->Draw(ui_->GetInstructionText());
	}


	// ポストエフェクト
	render_->Draw(transition_->GetPostEffect());

}
