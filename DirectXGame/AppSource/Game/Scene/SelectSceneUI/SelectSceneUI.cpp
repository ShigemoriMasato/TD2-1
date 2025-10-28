#include "SelectSceneUI.h"
#include <cmath>
#include <numbers>

SelectSceneUI::SelectSceneUI() {
}

SelectSceneUI::~SelectSceneUI() {
}

void SelectSceneUI::Initialize(Camera* camera, TextureManager* textureManager) {

	// テクスチャの読み込み
	leftArrowReleaseTextureHandle_ = textureManager->LoadTexture("Assets/Texture/SelectScene/ReleaseA.png");
	leftArrowTriggerTextureHandle_ = textureManager->LoadTexture("Assets/Texture/SelectScene/TriggerA.png");
	rightArrowReleaseTextureHandle_ = textureManager->LoadTexture("Assets/Texture/SelectScene/ReleaseD.png");
	rightArrowTriggerTextureHandle_ = textureManager->LoadTexture("Assets/Texture/SelectScene/TriggerD.png");
	stageSelectTextureHandle_ = textureManager->LoadTexture("Assets/Texture/SelectScene/StageSelect.png");

	// グラデーション背景
	gradientBackground_ = std::make_unique<DrawResource>();
	gradientBackground_->Initialize(ShapeType::Plane);
	gradientBackground_->camera_ = camera;
	gradientBackground_->position_ = { 0.0f, 0.0f, 200.0f };
	gradientBackground_->scale_ = { 1280.0f, 720.0f, 1.0f };
	gradientBackground_->color_ = 0x0f1419ff;  // 濃い青黒色

	// タイトルテキスト「STAGE SELECT」
	titleText_ = std::make_unique<DrawResource>();
	titleText_->Initialize(ShapeType::Plane);
	titleText_->camera_ = camera;
	titleText_->position_ = { 0.0f, 280.0f, 40.0f };
	titleText_->scale_ = { 400.0f, 60.0f, 1.0f };
	titleText_->color_ = 0xffffffff;
	titleText_->SetTextureHandle(stageSelectTextureHandle_);

	// 選択フレーム
	selectionFrame_ = std::make_unique<DrawResource>();
	selectionFrame_->Initialize(ShapeType::Plane);
	selectionFrame_->camera_ = camera;
	selectionFrame_->position_ = { 0.0f, 0.0f, 45.0f };  // ステージの後ろ
	selectionFrame_->scale_ = { 640.0f, 360.0f, 1.0f };
	selectionFrame_->color_ = 0x6a8a9a22;  // 半透明の青灰色
	selectionFrame_->psoConfig_.depthStencilID = DepthStencilID::Transparent;

	// 左矢印
	leftArrow_ = std::make_unique<DrawResource>();
	leftArrow_->Initialize(ShapeType::Plane);
	leftArrow_->camera_ = camera;
	leftArrow_->position_ = { -500.0f, 0.0f, 40.0f };
	leftArrow_->scale_ = { 128.0f, 128.0f, 1.0f };
	leftArrow_->color_ = 0xffffffff;
	leftArrow_->SetTextureHandle(leftArrowReleaseTextureHandle_);

	// 右矢印
	rightArrow_ = std::make_unique<DrawResource>();
	rightArrow_->Initialize(ShapeType::Plane);
	rightArrow_->camera_ = camera;
	rightArrow_->position_ = { 500.0f, 0.0f, 40.0f };
	rightArrow_->scale_ = { 128.0f, 128.0f, 1.0f };
	rightArrow_->color_ = 0xffffffff;
	rightArrow_->SetTextureHandle(rightArrowReleaseTextureHandle_);

	// 操作説明テキスト
	instructionText_ = std::make_unique<DrawResource>();
	instructionText_->Initialize(ShapeType::Plane);
	instructionText_->camera_ = camera;
	instructionText_->position_ = { 0.0f, -300.0f, 40.0f };
	instructionText_->scale_ = { 500.0f, 40.0f, 1.0f };
	instructionText_->color_ = 0x8a9aaaff;  // やや暗い灰色
}

void SelectSceneUI::Update(float deltaTime, const Vector3& centerPosition) {

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
	selectionFrame_->position_ = centerPosition;
	selectionFrame_->position_.z = 45.0f;
}

void SelectSceneUI::UpdateArrowTextures(bool isLeftPressed, bool isRightPressed) {

	// 左キーが押されている間はTriggerテクスチャに変更
	if (isLeftPressed) {
		leftArrow_->SetTextureHandle(leftArrowTriggerTextureHandle_);
	} else {
		leftArrow_->SetTextureHandle(leftArrowReleaseTextureHandle_);
	}

	// 右キーが押されている間はTriggerテクスチャに変更
	if (isRightPressed) {
		rightArrow_->SetTextureHandle(rightArrowTriggerTextureHandle_);
	} else {
		rightArrow_->SetTextureHandle(rightArrowReleaseTextureHandle_);
	}
}

void SelectSceneUI::UpdateArrowVisibility(bool canMoveLeft, bool canMoveRight) {
	// 左矢印の表示/非表示（アルファ値で制御）
	if (canMoveLeft) {
		leftArrow_->color_ = 0xffffffff;  // 完全表示
	} else {
		leftArrow_->color_ = 0xffffff00;  // 完全透明（非表示）
	}

	// 右矢印の表示/非表示（アルファ値で制御）
	if (canMoveRight) {
		rightArrow_->color_ = 0xffffffff;  // 完全表示
	} else {
		rightArrow_->color_ = 0xffffff00;  // 完全透明（非表示）
	}
}
