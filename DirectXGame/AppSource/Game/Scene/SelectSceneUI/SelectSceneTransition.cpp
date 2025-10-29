#include "SelectSceneTransition.h"
#include "StageCarousel.h"
#include <algorithm>
#include <cmath>
#include <numbers>

SelectSceneTransition::SelectSceneTransition() {
}

SelectSceneTransition::~SelectSceneTransition() {
}

void SelectSceneTransition::Initialize() {

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

	// SlowMotion効果の初期化
	postEffect_->data_.slowMotion.chromaticAberration = 0.0f;
	postEffect_->data_.slowMotion.vignetteStrength = 0.0f;
	postEffect_->data_.slowMotion.saturation = 1.0f;
	postEffect_->data_.slowMotion.intensity = 0.0f;

	// フェードイン状態の初期化
	isFadingIn_ = true;
	fadeInTimer_ = 0.0f;
}

bool SelectSceneTransition::UpdateFadeIn(float deltaTime) {

	// フェードインタイマーを進める
	fadeInTimer_ += deltaTime;
	float progress = std::min(fadeInTimer_ / kFadeInDuration, 1.0f);

	// progressを逆にして1.0→0.0にする
	postEffect_->data_.gridTransition.progress = 1.0f - progress;
	
	// フェードイン中は走査線を表示しない
	postEffect_->SetJobs(PostEffectJob::GridTransition);

	// フェードイン完了
	if (fadeInTimer_ >= kFadeInDuration) {
		isFadingIn_ = false;
		postEffect_->data_.gridTransition.progress = 0.0f;
		// フェードイン完了後は常時走査線のみ
		postEffect_->SetJobs(PostEffectJob::ConstantScanline);
		return true;
	}

	return false;
}

bool SelectSceneTransition::UpdateFadeOut(float deltaTime) {

	// フェードアウトタイマーを進める
	fadeOutTimer_ += deltaTime;
	float progress = std::min(fadeOutTimer_ / kFadeOutDuration, 1.0f);

	// グリッドトランジションを0.0→1.0に進める
	postEffect_->data_.gridTransition.progress = progress;
	postEffect_->data_.gridTransition.gridSize = 16.0f;
	postEffect_->data_.gridTransition.fadeColor = 0.0f;  // 黒にフェード
	postEffect_->data_.gridTransition.pattern = 4.0f; // 波紋状パターン
	postEffect_->SetJobs(PostEffectJob::GridTransition);

	// フェードアウト完了
	if (fadeOutTimer_ >= kFadeOutDuration) {
		return true;
	}

	return false;
}

bool SelectSceneTransition::UpdateZoomIn(float deltaTime, StageCarousel* carousel, int selectedStageIndex) {

	// ズームインタイマーを進める
	zoomInTimer_ += deltaTime;
	float progress = std::min(zoomInTimer_ / kZoomInDuration, 1.0f);
	float easedProgress = EaseInCubic(progress);  // 加速しながらズーム

	// 選択されたステージを取得
	DrawResource* selectedStage = carousel->GetStagePreview(selectedStageIndex);

	if (selectedStage) {
		// スケールを拡大
		Vector3 currentScale = selectedStage->scale_;
		Vector3 newScale;
		newScale.x = currentScale.x + (kZoomScale.x - currentScale.x) * easedProgress;
		newScale.y = currentScale.y + (kZoomScale.y - currentScale.y) * easedProgress;
		newScale.z = 1.0f;

		// カラーを少し明るく
		float brightness = 1.0f + (0.5f * std::sin(progress * std::numbers::pi_v<float>));  // 最大1.5倍
		carousel->UpdateZoomEffect(selectedStageIndex, newScale, brightness);
	}

	// 他のステージを画面外に移動（最初の一回で大きく移動）
	if (zoomInTimer_ <= deltaTime) {
		// ズームイン開始時に他のステージを一気に画面外に移動
		carousel->MoveOtherStagesOffScreen(selectedStageIndex, -5000.0f);
	}

	// ズームイン完了で待機時間開始
	if (zoomInTimer_ >= kZoomInDuration) {
		isZoomingIn_ = false;
		isWaitingAfterZoom_ = true;
		zoomWaitTimer_ = 0.0f;
		return true;
	}

	return false;
}

bool SelectSceneTransition::UpdateZoomWait(float deltaTime, StageCarousel* carousel, int selectedStageIndex) {

	// 待機タイマーを進める
	zoomWaitTimer_ += deltaTime;

	// 選択されたステージを取得してフラッシュ効果を継続
	DrawResource* selectedStage = carousel->GetStagePreview(selectedStageIndex);

	// 待機中はゆっくりと明るさを脈動させる
	if (selectedStage) {
		float pulseProgress = zoomWaitTimer_ / kZoomWaitDuration;
		float brightness = 1.0f + (0.2f * std::sin(pulseProgress * std::numbers::pi_v<float> * 4.0f));  // ゆっくり脈動
		carousel->UpdateZoomEffect(selectedStageIndex, selectedStage->scale_, brightness);
	}

	// 待機時間完了でフェードアウト開始
	if (zoomWaitTimer_ >= kZoomWaitDuration) {
		isWaitingAfterZoom_ = false;
		return true;
	}

	return false;
}

void SelectSceneTransition::StartZoomIn() {
	isZoomingIn_ = true;
	zoomInTimer_ = 0.0f;
}

void SelectSceneTransition::StartFadeOut() {
	isFadingOut_ = true;
	fadeOutTimer_ = 0.0f;
}

float SelectSceneTransition::EaseInCubic(float t) {
	return t * t * t;
}

float SelectSceneTransition::EaseOutCubic(float t) {
	return 1.0f - std::pow(1.0f - t, 3.0f);
}

void SelectSceneTransition::UpdateBackgroundGrid(float deltaTime) {
	if (!isBackgroundGridAnimating_) return;

	// タイマーを進める
	backgroundGridTimer_ += deltaTime;
	float progress = std::min(backgroundGridTimer_ / kBackgroundGridDuration, 1.0f);
	
	// EaseOutでスムーズに終わる
	float easedProgress = EaseOutCubic(progress);

	// 0から最大強度まで上がってから0に戻る（山型のカーブ）
	float intensity = std::sin(progress * std::numbers::pi_v<float>) * kBackgroundGridIntensity;

	// グリッドトランジションのパラメータを設定
	postEffect_->data_.gridTransition.progress = intensity;
	postEffect_->data_.gridTransition.gridSize = 20.0f;  // 細かいグリッド
	postEffect_->data_.gridTransition.fadeColor = 0.0f;
	postEffect_->data_.gridTransition.pattern = 0.0f;  // 波紋状パターン

	// 背景グリッドアニメーション中は常にGridTransitionを使用
	// ConstantScanlineは一時的に無効化される
	postEffect_->SetJobs(PostEffectJob::GridTransition);

	// アニメーション完了
	if (backgroundGridTimer_ >= kBackgroundGridDuration) {
		isBackgroundGridAnimating_ = false;
		backgroundGridTimer_ = 0.0f;
		postEffect_->data_.gridTransition.progress = 0.0f;
		
		// 常時走査線に戻る（グリッチは別の場所で管理）
		postEffect_->SetJobs(PostEffectJob::ConstantScanline);
	}
}

void SelectSceneTransition::TriggerBackgroundGrid() {
	// 他のトランジション中は発動しない
	if (isFadingIn_ || isFadingOut_ || isZoomingIn_ || isWaitingAfterZoom_) {
		return;
	}

	// アニメーションを開始
	isBackgroundGridAnimating_ = true;
	backgroundGridTimer_ = 0.0f;
}
