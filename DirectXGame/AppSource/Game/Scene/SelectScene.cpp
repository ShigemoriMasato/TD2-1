#include "SelectScene.h"
#include "GameScene.h"
#include <Game/LevelLoader.h>
#include <algorithm>
#include <Math/MyMath.h>
#include <cmath>
#include <cstdlib>

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

	// 常時走査線の初期化
	transition_->GetPostEffect()->data_.constantScanline.intensity = 0.25f;  // 控えめな強度
	transition_->GetPostEffect()->data_.constantScanline.speed = 1.0f;
	transition_->GetPostEffect()->data_.constantScanline.lineWidth = 0.002f;
	transition_->GetPostEffect()->data_.constantScanline.time = 0.0f;
	scanlineTime_ = 0.0f;

	// グリッチエフェクトの初期化
	transition_->GetPostEffect()->data_.glitch.intensity = 0.0f;
	transition_->GetPostEffect()->data_.glitch.rgbSplit = 0.0f;
	transition_->GetPostEffect()->data_.glitch.scanlineIntensity = 0.0f;
	transition_->GetPostEffect()->data_.glitch.blockIntensity = 0.0f;
	transition_->GetPostEffect()->data_.glitch.time = 0.0f;

	// グリッチ状態の初期化
	glitchTimer_ = 0.0f;
	glitchDuration_ = 0.0f;
	isGlitching_ = false;
	// 初回のグリッチ発生時刻をランダムに設定（起動後1〜3秒後）
	nextGlitchTime_ = 1.0f + (static_cast<float>(std::rand()) / RAND_MAX) * 2.0f;

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

	// 前回選択されたステージインデックスを復元（nextLevelIndexから計算）
	int lastSelectedIndex = static_cast<int>(commonData->nextLevelIndex_);
	// 範囲チェック
	if (lastSelectedIndex >= 0 && lastSelectedIndex < (int)LevelIndex::kNumLevels) {
		stageCarousel_->SetSelectedStageIndex(lastSelectedIndex);
	}

	// スコア表示の初期化（5つのステージ分）
	stageScores_.resize(5);
	scoreLabels_.resize(5);
	for (int i = 0; i < 5; ++i) {
		stageScores_[i] = std::make_unique<TimerResource>();
		// 4桁表示で初期化（スコアは0〜9999を想定）
		stageScores_[i]->Initialize(Vector3{ 45.0f, 50.0f, 1.0f }, 7, false);
		stageScores_[i]->SetCamera(uiCamera_.get());
		stageScores_[i]->SetColor(0xffffffee);  // やや半透明の白
		
		// commonDataからスコアを読み取り、初期表示
		int score = commonData->scoreManager_->GetScore(i);
		stageScores_[i]->Update(score);

		// スコアラベル（"SCORE"テクスチャ）の初期化
		scoreLabels_[i] = std::make_unique<DrawResource>();
		scoreLabels_[i]->Initialize(ShapeType::Plane);
		scoreLabels_[i]->camera_ = uiCamera_.get();
		scoreLabels_[i]->scale_ = { 300.0f, 50.0f, 1.0f };  // ラベルのサイズ
		scoreLabels_[i]->color_ = 0xffffffee;  // やや半透明の白
		scoreLabels_[i]->SetTextureHandle("Assets/Texture/score.png");
		scoreLabels_[i]->psoConfig_.depthStencilID = DepthStencilID::Transparent;
	}

	// 入力ハンドラー初期化
	inputHandler_ = std::make_unique<SelectSceneInputHandler>();
	inputHandler_->Initialize(audio_);

	// ステージ選択変更時のコールバック設定
	inputHandler_->SetOnStageChangeCallback([this](int direction) -> bool {
		int currentIndex = stageCarousel_->GetSelectedStageIndex();
		int newIndex = currentIndex + direction;

		// 両端でストップ
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

	// アニメーション状態チェックのコールバック設定
	inputHandler_->SetIsAnimatingCallback([this]() -> bool {
		return stageCarousel_->IsAnimating();
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

	// エフェクトの更新（背景グリッドの後に実行）
	// 背景グリッドアニメーション中でない場合のみ更新
	if (!transition_->IsBackgroundGridAnimating()) {
		// 常時走査線の更新
		UpdateConstantScanline(deltaTime);
		
		// グリッチエフェクトの更新
		UpdateGlitch(deltaTime);
	}

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

	// スコア表示の位置を各ステージプレビューの下に配置
	for (int i = 0; i < 5; ++i) {
		DrawResource* stagePreview = stageCarousel_->GetStagePreview(i);
		if (stagePreview) {
			Vector3 scorePosition = stagePreview->position_;
			// ステージプレビューの下
			scorePosition.y = -250.0f;
			// スコア表示を少し手前に配置
			scorePosition.z = 45.0f;
			
			// スコアラベルの位置
			Vector3 labelPosition = scorePosition;
			labelPosition.x -= 220.0f;
			scoreLabels_[i]->position_ = labelPosition;
			
			// 数字の位置（ラベルの右側に配置）
			scorePosition.x += 320.0f;  // ラベルの右端から少し離す
			stageScores_[i]->SetPosition(scorePosition);
			
			// 位置を反映させるために再度Updateを呼ぶ
			int score = commonData->bestScore_[i];
			stageScores_[i]->Update(score);
			
			// スコアの色をステージの選択状態に合わせる
			if (i == stageCarousel_->GetSelectedStageIndex()) {
				stageScores_[i]->SetColor(0xffffffee);  // 選択中は明るく
				scoreLabels_[i]->color_ = 0xffffffee;
			} else {
				stageScores_[i]->SetColor(0x808080cc);  // 非選択時は暗く
				scoreLabels_[i]->color_ = 0x808080cc;
			}
		}
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

void SelectScene::UpdateConstantScanline(float deltaTime) {
	// トランジション中やズーム中は走査線を表示しない
	if (transition_->IsFadingIn() || transition_->IsFadingOut() || 
		transition_->IsZoomingIn() || transition_->IsWaitingAfterZoom()) {
		return;
	}

	// 背景グリッドアニメーション中は走査線を表示しない
	if (transition_->IsBackgroundGridAnimating()) {
		return;
	}

	// 時間パラメータを更新
	scanlineTime_ += deltaTime;
	transition_->GetPostEffect()->data_.constantScanline.time = scanlineTime_;
}

void SelectScene::UpdateGlitch(float deltaTime) {
	// フェード中やズーム中はグリッチを発生させない
	if (transition_->IsFadingIn() || transition_->IsFadingOut() || 
		transition_->IsZoomingIn() || transition_->IsWaitingAfterZoom()) {
		isGlitching_ = false;
		// グリッチエフェクトをリセット
		transition_->GetPostEffect()->data_.glitch.intensity = 0.0f;
		transition_->GetPostEffect()->data_.glitch.rgbSplit = 0.0f;
		transition_->GetPostEffect()->data_.glitch.scanlineIntensity = 0.0f;
		transition_->GetPostEffect()->data_.glitch.blockIntensity = 0.0f;
		return;
	}

	// 背景グリッドアニメーション中はグリッチをスキップ
	if (transition_->IsBackgroundGridAnimating()) {
		isGlitching_ = false;
		// グリッチエフェクトをリセット
		transition_->GetPostEffect()->data_.glitch.intensity = 0.0f;
		transition_->GetPostEffect()->data_.glitch.rgbSplit = 0.0f;
		transition_->GetPostEffect()->data_.glitch.scanlineIntensity = 0.0f;
		transition_->GetPostEffect()->data_.glitch.blockIntensity = 0.0f;
		return;
	}

	// 時間パラメータを常に更新（ランダム性のため）
	transition_->GetPostEffect()->data_.glitch.time += deltaTime;

	if (!isGlitching_) {
		// グリッチ待機中
		glitchTimer_ += deltaTime;

		// 次のグリッチ発生時刻に到達したらグリッチ開始
		if (glitchTimer_ >= nextGlitchTime_) {
			// グリッチ開始
			isGlitching_ = true;
			glitchDuration_ = 0.0f;
			glitchTimer_ = 0.0f;

			// グリッチの継続時間をランダム化（0.2〜0.5秒）
			glitchMaxDuration_ = 0.2f + (static_cast<float>(std::rand()) / RAND_MAX) * 0.3f;
			
			// 次のグリッチ発生時刻を設定（基本間隔±ランダム幅）
			float randomFactor = (static_cast<float>(std::rand()) / RAND_MAX) * 2.0f - 1.0f;
			nextGlitchTime_ = glitchInterval_ + (randomFactor * glitchIntervalVariation_);
			
			// 最低1秒は空ける
			if (nextGlitchTime_ < 1.0f) {
				nextGlitchTime_ = 1.0f;
			}
		}
	} else {
		// グリッチ中
		glitchDuration_ += deltaTime;

		// グリッチの強度を計算（よりスムーズなカーブ）
		float progress = glitchDuration_ / glitchMaxDuration_;
		
		// より緩やかなフェードイン/アウト
		float intensity;
		if (progress < 0.2f) {
			// 最初の20%でフェードイン
			float t = progress / 0.2f;
			intensity = t * t * (3.0f - 2.0f * t); // smoothstep
		} else if (progress > 0.8f) {
			// 最後の20%でフェードアウト
			float t = (1.0f - progress) / 0.2f;
			intensity = t * t * (3.0f - 2.0f * t); // smoothstep
		} else {
			// 中間60%は最大強度を維持
			intensity = 1.0f;
		}

		// 強度にランダムなスパイクを追加
		float randomSpike = 1.0f + (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * 0.3f;
		intensity *= randomSpike;
		intensity = std::min(intensity, 1.0f);

		// グリッチパラメータを設定
		transition_->GetPostEffect()->data_.glitch.intensity = intensity * 0.85f;
		transition_->GetPostEffect()->data_.glitch.rgbSplit = intensity * 0.9f;
		transition_->GetPostEffect()->data_.glitch.scanlineIntensity = intensity * 0.75f;
		transition_->GetPostEffect()->data_.glitch.blockIntensity = intensity * 0.6f;

		// 常時走査線とグリッチを組み合わせる
		transition_->GetPostEffect()->SetJobs(PostEffectJob::ConstantScanline);

		// グリッチ終了判定
		if (glitchDuration_ >= glitchMaxDuration_) {
			isGlitching_ = false;
			glitchDuration_ = 0.0f;

			// グリッチエフェクトをリセット
			transition_->GetPostEffect()->data_.glitch.intensity = 0.0f;
			transition_->GetPostEffect()->data_.glitch.rgbSplit = 0.0f;
			transition_->GetPostEffect()->data_.glitch.scanlineIntensity = 0.0f;
			transition_->GetPostEffect()->data_.glitch.blockIntensity = 0.0f;
			
			// 常時走査線のみに戻る
			transition_->GetPostEffect()->SetJobs(PostEffectJob::ConstantScanline);
		}
	}
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

	// スコア表示（ステージプレビューの後、UI要素の前）
	if (!transition_->IsZoomingIn() && !transition_->IsWaitingAfterZoom() && !transition_->IsFadingOut()) {
		// 現在選択中のステージのスコアのみ描画
		int selectedIndex = stageCarousel_->GetSelectedStageIndex();
		
		// スコアラベル（"SCORE"テクスチャ）を描画
		render_->Draw(scoreLabels_[selectedIndex].get());
		
		// スコアの数字を描画
		auto scoreResources = stageScores_[selectedIndex]->GetDrawResources();
		for (auto* resource : scoreResources) {
			render_->Draw(resource);
		}
	}

	// UI要素（最前面）
	if (!transition_->IsZoomingIn() && !transition_->IsWaitingAfterZoom() && !transition_->IsFadingOut()) {
		render_->Draw(ui_->GetTitleText());
		render_->Draw(ui_->GetLeftArrow());
		render_->Draw(ui_->GetRightArrow());
	}


	// ポストエフェクト
	render_->Draw(transition_->GetPostEffect());

}
