#include "TitleScene.h"
#include "SelectScene.h"
#include <cmath>
#include <cstdlib>

void TitleScene::Initialize()
{

	{
		//PostEffect初期化
		postEffect_ = std::make_unique<PostEffectResource>();
		postEffect_->Initialize();
		postEffect_->input_ = OffScreenIndex::Title;
		postEffect_->output_ = OffScreenIndex::SwapChain;

		// グリッドトランジション初期化
		postEffect_->data_.gridTransition.progress = 0.0f;
		postEffect_->data_.gridTransition.gridSize = 16.0f;  // 16x16グリッド
		postEffect_->data_.gridTransition.fadeColor = 0.0f;  // 黒にフェード
		postEffect_->data_.gridTransition.pattern = 0.0f;    // 波紋状パターン（外→内）
		
		// グリッチエフェクト初期化
		postEffect_->data_.glitch.intensity = 0.0f;
		postEffect_->data_.glitch.rgbSplit = 0.0f;
		postEffect_->data_.glitch.scanlineIntensity = 0.0f;
		postEffect_->data_.glitch.blockIntensity = 0.0f;
		postEffect_->data_.glitch.time = 0.0f;
	}

	// フェード状態の初期化
	isFading_ = false;
	fadeTimer_ = 0.0f;

	// グリッチ状態の初期化
	glitchTimer_ = 0.0f;
	glitchDuration_ = 0.0f;
	isGlitching_ = false;

	// アニメーション用タイマーの初期化
	totalTime_ = 0.0f;
	titleLogoAnimTime_ = 0.0f;
	spaceStartAnimTime_ = 0.0f;

	// UI用カメラの初期化（Orthographic投影）
	// 画面中央が原点(0,0)になる座標系
	{
		uiCamera_ = std::make_unique<Camera>();
		OrthographicDesc desc{};
		desc.left = -640.0f;      // 左端 -640
		desc.top = 360.0f;        // 上端 +360
		desc.right = 640.0f;      // 右端 +640
		desc.bottom = -360.0f;    // 下端 -360
		desc.nearClip = 0.0f;
		desc.farClip = 1000.0f;
		uiCamera_->SetProjectionMatrix(desc);

		// カメラのトランスフォームを設定（UI用なので位置は原点）
		Transform cameraTransform{};
		cameraTransform.position = { 0.0f, 0.0f, 0.0f };
		cameraTransform.rotation = { 0.0f, 0.0f, 0.0f };
		cameraTransform.scale = { 1.0f, 1.0f, 1.0f };
		uiCamera_->SetTransform(cameraTransform);
		uiCamera_->MakeMatrix();
	}

	// テクスチャの読み込み
	{
		titleTextureHandle_ = textureManager_->LoadTexture("Assets/Texture/TitleScene/title.png");
		spaceStartTextureHandle_ = textureManager_->LoadTexture("Assets/Texture/TitleScene/spaceStart.png");
		backgroundTextureHandle_ = textureManager_->LoadTexture("Assets/Texture/TitleScene/titleBackGround.png");
	}

	// タイトルロゴの初期化
	{
		titleLogo_ = std::make_unique<DrawResource>();
		titleLogo_->Initialize(ShapeType::Plane);
		titleLogo_->SetTextureHandle(titleTextureHandle_);
		titleLogo_->camera_ = uiCamera_.get();

		// 初期位置を保存
		titleLogoInitialPos_ = { 0.0f, 160.0f, 0.0f };

		// 画面中央上部に配置（原点は画面中央）
		// アニメーション開始位置（上方にオフセット）
		titleLogo_->position_ = { titleLogoInitialPos_.x, titleLogoInitialPos_.y + titleLogoSlideDistance_, titleLogoInitialPos_.z };
		titleLogo_->scale_ = { 760.0f, 270.0f, 1.0f };
		titleLogo_->rotate_ = { 0.0f, 0.0f, 0.0f };

		// 初期状態は透明
		titleLogo_->color_ = 0x00ffffff;
	}

	// Space Startテキストの初期化
	{
		spaceStart_ = std::make_unique<DrawResource>();
		spaceStart_->Initialize(ShapeType::Plane);
		spaceStart_->SetTextureHandle(spaceStartTextureHandle_);
		spaceStart_->camera_ = uiCamera_.get();

		// 画面中央下部に配置（原点は画面中央）
		spaceStart_->position_ = { 0.0f, -160.0f, 0.0f };  // Y-で下方向
		spaceStart_->scale_ = { 633.0f, 66.0f, 1.0f };
		spaceStart_->rotate_ = { 0.0f, 0.0f, 0.0f };

		// 初期状態は透明
		spaceStart_->color_ = 0x00ffffff;
	}

	/// 背景の初期化
	{
		background_ = std::make_unique<DrawResource>();
		background_->Initialize(ShapeType::Plane);
		background_->SetTextureHandle(backgroundTextureHandle_);
		background_->camera_ = uiCamera_.get();

		// 画面全体を覆うように配置
		background_->position_ = { 0.0f, 0.0f, 100.0f };  // Z+で手前に配置
		background_->scale_ = { 1280.0f, 720.0f, 1.0f };
		background_->rotate_ = { 0.0f, 0.0f, 0.0f };

		background_->color_ = 0xffffffff;
	}

	//BGMの再生
	if (!audio_->IsPlay(commonData->bgmPlayHandle_)) {
		int soundHandle = audio_->Load("BGM/Normal.mp3");

		if (commonData->bgmPlayHandle_ / 10 != soundHandle) {
			audio_->Stop(commonData->bgmPlayHandle_);
			commonData->bgmPlayHandle_ = audio_->Play(soundHandle, true);
		}
	}
}

std::unique_ptr<BaseScene> TitleScene::Update()
{
	float deltaTime = fpsObserver_->GetDeltatime();

	// 総経過時間の更新
	totalTime_ += deltaTime;

	// グリッチエフェクトの更新（常に更新）
	UpdateGlitch(deltaTime);

	// UIアニメーションの更新（フェード中でない場合のみ）
	if (!isFading_) {
		UpdateTitleLogoAnimation(deltaTime);
		UpdateSpaceStartAnimation(deltaTime);
	}

	// キー状態を取得
	auto keys = commonData->keyManager_->GetKeyStates();

	// スペースキー（Actionキー）でグリッドトランジション開始
	if (!isFading_ && keys[Key::Action]) {
		int se = audio_->Load("start.mp3");
		audio_->Play(se, false);
		isFading_ = true;
		fadeTimer_ = 0.0f;
	}

	// フェード処理の更新
	UpdateFade(deltaTime);

	// トランジション完了でゲームシーンへ
	if (isFading_ && fadeTimer_ >= fadeDuration_) {
		// ゲーム開始時は常にステージ1（Level0）からスタート
		commonData->nextLevelIndex_ = LevelIndex::Level0;
		return std::make_unique<SelectScene>();
	}

	return std::unique_ptr<BaseScene>();
}

void TitleScene::UpdateTitleLogoAnimation(float deltaTime)
{
	titleLogoAnimTime_ += deltaTime;

	// フェードイン & スライドダウン（最初の1.2秒）
	if (titleLogoAnimTime_ <= titleLogoFadeDuration_) {
		float t = titleLogoAnimTime_ / titleLogoFadeDuration_;
		float easedT = EaseOutCubic(t);

		// スライドダウン
		float slideOffset = titleLogoSlideDistance_ * (1.0f - easedT);
		titleLogo_->position_.y = titleLogoInitialPos_.y + slideOffset;

		// フェードイン
		uint8_t alpha = static_cast<uint8_t>(255.0f * easedT);
		titleLogo_->color_ = (alpha << 24) | 0x00ffffff;
	} else {
		// フェードイン完了後は浮遊アニメーション
		float floatTime = titleLogoAnimTime_ - titleLogoFadeDuration_;
		float floatOffset = std::sin(floatTime * titleLogoFloatSpeed_) * titleLogoFloatAmplitude_;

		titleLogo_->position_.y = titleLogoInitialPos_.y + floatOffset;
		titleLogo_->color_ = 0xffffffff;
	}
}

void TitleScene::UpdateSpaceStartAnimation(float deltaTime)
{
	spaceStartAnimTime_ += deltaTime;

	// 遅延時間中は非表示
	if (spaceStartAnimTime_ < spaceStartDelayTime_) {
		spaceStart_->color_ = 0x00ffffff;
		return;
	}

	float effectiveTime = spaceStartAnimTime_ - spaceStartDelayTime_;

	// フェードイン（遅延後から0.8秒）
	if (effectiveTime <= spaceStartFadeDuration_) {
		float t = effectiveTime / spaceStartFadeDuration_;
		float easedT = EaseOutCubic(t);

		uint8_t alpha = static_cast<uint8_t>(255.0f * easedT);
		spaceStart_->color_ = (alpha << 24) | 0x00ffffff;
	} else {
		// フェードイン完了後はパルスアニメーション
		float pulseTime = effectiveTime - spaceStartFadeDuration_;
		float pulse = EaseInOutSine(std::fmod(pulseTime * spaceStartPulseSpeed_, 1.0f));

		// パルス（最小〜最大の間で振動）
		float alpha = spaceStartPulseMin_ + (spaceStartPulseMax_ - spaceStartPulseMin_) * pulse;
		uint8_t alphaValue = static_cast<uint8_t>(255.0f * alpha);
		spaceStart_->color_ = (alphaValue << 24) | 0x00ffffff;
	}
}

void TitleScene::UpdateFade(float deltaTime)
{
	if (isFading_) {
		// フェードタイマーを進める
		fadeTimer_ += deltaTime;
		float progress = std::min(fadeTimer_ / fadeDuration_, 1.0f);

		// ポストエフェクトの進行度を更新
		postEffect_->data_.gridTransition.progress = progress;
		postEffect_->SetJobs(PostEffectJob::GridTransition);
	} else if (!isGlitching_) {
		// トランジションしていない&グリッチ中でない時は通常描画
		postEffect_->data_.gridTransition.progress = 0.0f;
		postEffect_->SetJobs(PostEffectJob::None);
	}
}

void TitleScene::UpdateGlitch(float deltaTime)
{
	// フェード中はグリッチを発生させない
	if (isFading_) {
		isGlitching_ = false;
		return;
	}

	// 時間パラメータを常に更新（ランダム性のため）
	postEffect_->data_.glitch.time += deltaTime;

	if (!isGlitching_) {
		// グリッチ待機中
		glitchTimer_ += deltaTime;

		// ランダムな間隔でグリッチを発生
		// 基本間隔に±1秒のランダム性を追加
		float randomOffset = (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * 2.0f;
		float actualInterval = glitchInterval_ + randomOffset;

		if (glitchTimer_ >= actualInterval) {
			// グリッチ開始
			isGlitching_ = true;
			glitchDuration_ = 0.0f;
			glitchTimer_ = 0.0f;

			// グリッチの継続時間をランダム化（0.05〜0.15秒）
			glitchMaxDuration_ = 0.05f + (static_cast<float>(std::rand()) / RAND_MAX) * 0.1f;
		}
	} else {
		// グリッチ中
		glitchDuration_ += deltaTime;

		// グリッチの強度を計算（最初と最後で弱く、中間で強い）
		float progress = glitchDuration_ / glitchMaxDuration_;
		float intensity = std::sin(progress * 3.14159265358979323846f); // 0→1→0の曲線

		// グリッチパラメータを設定
		postEffect_->data_.glitch.intensity = intensity * 0.7f;
		postEffect_->data_.glitch.rgbSplit = intensity * 0.8f;
		postEffect_->data_.glitch.scanlineIntensity = intensity * 0.6f;
		postEffect_->data_.glitch.blockIntensity = intensity * 0.5f;

		// グリッチエフェクトをセット
		postEffect_->SetJobs(PostEffectJob::Glitch);

		// グリッチ終了判定
		if (glitchDuration_ >= glitchMaxDuration_) {
			isGlitching_ = false;
			glitchDuration_ = 0.0f;

			// グリッチエフェクトをリセット
			postEffect_->data_.glitch.intensity = 0.0f;
			postEffect_->data_.glitch.rgbSplit = 0.0f;
			postEffect_->data_.glitch.scanlineIntensity = 0.0f;
			postEffect_->data_.glitch.blockIntensity = 0.0f;
			postEffect_->SetJobs(PostEffectJob::None);
		}
	}
}

float TitleScene::EaseOutCubic(float t)
{
	float f = t - 1.0f;
	return f * f * f + 1.0f;
}

float TitleScene::EaseInOutSine(float t)
{
	return -(std::cos(3.14159265358979323846f * t) - 1.0f) / 2.0f;
}

void TitleScene::Draw()
{
	render_->PreDraw(OffScreenIndex::Title);

	// UI描画
	render_->Draw(background_.get());
	render_->Draw(titleLogo_.get());
	render_->Draw(spaceStart_.get());


	render_->Draw(postEffect_.get());
}