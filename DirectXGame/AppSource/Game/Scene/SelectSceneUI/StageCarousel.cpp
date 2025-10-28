#include "StageCarousel.h"
#include <Game/LevelLoader.h>
#include <algorithm>
#include <cmath>

StageCarousel::StageCarousel() {
	// 5個のステージアニメーションデータを初期化
	stageAnimations_.resize(5);
}

StageCarousel::~StageCarousel() {
}

void StageCarousel::Initialize(Camera* camera, const OffScreenIndex* offScreenIndices, OffScreenManager* offScreenManager) {

	// 初期配置の位置テーブル
	const Vector3 initialPositions[] = {
		kCenterPosition,    // Stage 0: 中央（選択状態）
		kRightPosition,       // Stage 1: 右側
		kFarRightPosition,    // Stage 2: 右遠く
		kFarRightPosition,    // Stage 3: 右遠く
		kFarRightPosition,    // Stage 4: 右遠く
	};

	for (int i = 0; i < 5; ++i) {
		// DrawResourceの作成と初期化
		stagePreviews_.push_back(std::make_unique<DrawResource>());
		stagePreviews_.back()->Initialize(ShapeType::Plane);
		stagePreviews_.back()->SetTextureHandle(offScreenManager->GetOffScreenData(offScreenIndices[i])->GetTextureGPUHandle());
		stagePreviews_.back()->camera_ = camera;
		stagePreviews_.back()->rotate_ = { 0.0f, 0.0f, 0.0f };

		// 最初のステージ（i == 0）は選択状態、それ以外は非選択状態
		bool isSelected = (i == 0);
		stagePreviews_.back()->position_ = initialPositions[i];
		stagePreviews_.back()->scale_ = isSelected ? kSelectedScale : kUnselectedScale;
		stagePreviews_.back()->color_ = isSelected ? kSelectedColor : kUnselectedColor;

		// アニメーションデータの初期化
		stageAnimations_[i].currentPosition = initialPositions[i];
		stageAnimations_[i].targetPosition = initialPositions[i];
		stageAnimations_[i].currentScale = isSelected ? kSelectedScale : kUnselectedScale;
		stageAnimations_[i].targetScale = isSelected ? kSelectedScale : kUnselectedScale;
		stageAnimations_[i].currentColor = isSelected ? kSelectedColor : kUnselectedColor;
		stageAnimations_[i].targetColor = isSelected ? kSelectedColor : kUnselectedColor;
		stageAnimations_[i].animationTimer = 0.0f;
		stageAnimations_[i].isAnimating = false;
	}

	selectedStageIndex_ = 0;
	previousStageIndex_ = 0;
}

void StageCarousel::Update(float deltaTime, bool skipAnimation) {

	// スキップ指定がある場合はアニメーション更新をスキップ
	if (skipAnimation) return;

	// 全ステージのアニメーション更新（5個）
	for (int i = 0; i < 5; ++i) {
		if (stageAnimations_[i].isAnimating) {
			stageAnimations_[i].animationTimer += deltaTime;
			float t = std::min(stageAnimations_[i].animationTimer / kAnimationDuration, 1.0f);
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

void StageCarousel::SetSelectedStageIndex(int newIndex) {
	previousStageIndex_ = selectedStageIndex_;
	selectedStageIndex_ = newIndex;
	SetStagePreviewTargets();
}

DrawResource* StageCarousel::GetStagePreview(int index) {
	if (index >= 0 && index < static_cast<int>(stagePreviews_.size())) {
		return stagePreviews_[index].get();
	}
	return nullptr;
}

void StageCarousel::UpdateZoomEffect(int stageIndex, const Vector3& scale, float brightness) {
	if (stageIndex >= 0 && stageIndex < static_cast<int>(stagePreviews_.size())) {
		stagePreviews_[stageIndex]->scale_ = scale;

		// カラーを明るく
		uint8_t r = static_cast<uint8_t>(std::min(255.0f, 255.0f * brightness));
		uint8_t g = static_cast<uint8_t>(std::min(255.0f, 255.0f * brightness));
		uint8_t b = static_cast<uint8_t>(std::min(255.0f, 255.0f * brightness));
		stagePreviews_[stageIndex]->color_ = (r << 24) | (g << 16) | (b << 8) | 0xff;
	}
}

void StageCarousel::MoveOtherStagesOffScreen(int selectedIndex, float offset) {
	for (int i = 0; i < 5; ++i) {
		if (i != selectedIndex) {
			stagePreviews_[i]->position_.x += offset;
		}
	}
}

void StageCarousel::SetStagePreviewTargets() {

	for (int i = 0; i < 5; ++i) {
		int relativePos = i - selectedStageIndex_;  // 選択ステージからの相対位置

		// 相対位置に応じて目標値を設定
		if (relativePos == 0) {
			// 選択中のステージ: 中央
			stageAnimations_[i].targetPosition = kCenterPosition;
			stageAnimations_[i].targetScale = kSelectedScale;
			stageAnimations_[i].targetColor = kSelectedColor;
		} else if (relativePos == 1) {
			// 右隣のステージ
			stageAnimations_[i].targetPosition = kRightPosition;
			stageAnimations_[i].targetScale = kUnselectedScale;
			stageAnimations_[i].targetColor = kUnselectedColor;
		} else if (relativePos == -1) {
			// 左隣のステージ
			stageAnimations_[i].targetPosition = kLeftPosition;
			stageAnimations_[i].targetScale = kUnselectedScale;
			stageAnimations_[i].targetColor = kUnselectedColor;
		} else if (relativePos == 2) {
			// 右2つ目のステージ
			stageAnimations_[i].targetPosition = kFarRightPosition;
			stageAnimations_[i].targetScale = kUnselectedScale;
			stageAnimations_[i].targetColor = kUnselectedColor;
		} else if (relativePos == -2) {
			// 左2つ目のステージ
			stageAnimations_[i].targetPosition = kFarLeftPosition;
			stageAnimations_[i].targetScale = kUnselectedScale;
			stageAnimations_[i].targetColor = kUnselectedColor;
		} else {
			// それ以外は画面外（さらに遠く）
			if (relativePos > 0) {
				stageAnimations_[i].targetPosition = { kFarRightPosition.x + 700.0f * (relativePos - 2), 0.0f, 50.0f };
			} else {
				stageAnimations_[i].targetPosition = { kFarLeftPosition.x - 700.0f * (-relativePos - 2), 0.0f, 50.0f };
			}
			stageAnimations_[i].targetScale = kUnselectedScale;
			stageAnimations_[i].targetColor = kUnselectedColor;
		}

		// アニメーション開始
		stageAnimations_[i].isAnimating = true;
		stageAnimations_[i].animationTimer = 0.0f;
	}
}

float StageCarousel::EaseOutCubic(float t) {
	return 1.0f - std::powf(1.0f - t, 3.0f);
}
