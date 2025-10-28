#pragma once
#include <Render/Resource/DrawResource.h>
#include <Camera/Camera.h>
#include <Resource/Texture/TextureManager.h>
#include <Transform/Vector.h>
#include <memory>

/// <summary>
/// セレクトシーンのUI要素管理
/// タイトル、矢印、選択フレーム、背景などの管理と装飾アニメーション
/// </summary>
class SelectSceneUI {
public:
	SelectSceneUI();
	~SelectSceneUI();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="camera">UI用カメラ</param>
	/// <param name="textureManager">テクスチャマネージャー</param>
	void Initialize(Camera* camera, TextureManager* textureManager);

	/// <summary>
	/// UI装飾の更新（脈動アニメーションなど）
	/// </summary>
	/// <param name="deltaTime">デルタタイム</param>
	/// <param name="centerPosition">選択フレームの中央位置</param>
	void Update(float deltaTime, const Vector3& centerPosition);

	/// <summary>
	/// 矢印テクスチャを更新
	/// </summary>
	/// <param name="isLeftPressed">左キーが押されているか</param>
	/// <param name="isRightPressed">右キーが押されているか</param>
	void UpdateArrowTextures(bool isLeftPressed, bool isRightPressed);

	/// <summary>
	/// 描画すべきUI要素を取得
	/// </summary>
	DrawResource* GetGradientBackground() const { return gradientBackground_.get(); }
	DrawResource* GetTitleText() const { return titleText_.get(); }
	DrawResource* GetSelectionFrame() const { return selectionFrame_.get(); }
	DrawResource* GetLeftArrow() const { return leftArrow_.get(); }
	DrawResource* GetRightArrow() const { return rightArrow_.get(); }
	DrawResource* GetInstructionText() const { return instructionText_.get(); }

private:
	// UI要素
	std::unique_ptr<DrawResource> gradientBackground_;
	std::unique_ptr<DrawResource> titleText_;
	std::unique_ptr<DrawResource> selectionFrame_;
	std::unique_ptr<DrawResource> leftArrow_;
	std::unique_ptr<DrawResource> rightArrow_;
	std::unique_ptr<DrawResource> instructionText_;

	// アニメーションタイマー
	float decorationTimer_ = 0.0f;
	float arrowPulseTimer_ = 0.0f;

	// テクスチャハンドル
	int leftArrowReleaseTextureHandle_ = 0;
	int leftArrowTriggerTextureHandle_ = 0;
	int rightArrowReleaseTextureHandle_ = 0;
	int rightArrowTriggerTextureHandle_ = 0;
};
