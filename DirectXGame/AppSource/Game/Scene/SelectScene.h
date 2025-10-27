#pragma once
#include <Scene/Data/BaseScene.h>
#include <map>
#include <string>
#include <Scene/SceneManager.h>
#include <memory>

class SelectScene : public BaseScene {
public:

	SelectScene();
	~SelectScene();

	void Initialize() override;
	std::unique_ptr<BaseScene> Update() override;
	void Draw() override;

private:

	/// @brief フェードイン処理の更新
	void UpdateFadeIn(float deltaTime);

	/// @brief フェードアウト処理の更新
	void UpdateFadeOut(float deltaTime);

	/// @brief 入力処理
	void ProcessInput();

	/// @brief ステージプレビューのアニメーション更新
	void UpdateStagePreviewAnimation(float deltaTime);

	/// @brief ズームイン演出の更新
	void UpdateZoomInEffect(float deltaTime);

	/// @brief ズームイン後の待機時間更新
	void UpdateZoomWait(float deltaTime);

	/// @brief ステージプレビューの目標値を設定
	void SetStagePreviewTargets();

	/// @brief ズームイン演出の開始
	void StartZoomInEffect();

	/// @brief シーン遷移判定
	/// @return 次のシーンのポインタ（遷移しない場合はnullptr）
	std::unique_ptr<BaseScene> CheckSceneTransition();

	/// @brief イージング関数（EaseOutCubic）
	/// @param t 進行度（0.0～1.0）
	/// @return イージング適用後の値（0.0～1.0）
	float EaseOutCubic(float t);

	/// @brief イージング関数（EaseInCubic）
	/// @param t 進行度（0.0～1.0）
	/// @return イージング適用後の値（0.0～1.0）
	float EaseInCubic(float t);

	static inline bool firstLoad_ = false;

	std::map<int, std::string> stageMap_;
	std::unique_ptr<SceneManager> sceneManager_;

	// ポストエフェクト
	std::unique_ptr<PostEffectResource> postEffect_ = nullptr;

	// フェードイン処理用
	bool isFadingIn_ = true;
	float fadeInTimer_ = 0.0f;
	const float fadeInDuration_ = 1.5f; // フェードインにかける時間（秒）

	// フェードアウト処理用
	bool isFadingOut_ = false;
	float fadeOutTimer_ = 0.0f;
	const float fadeOutDuration_ = 2.0f; // フェードアウトにかける時間（秒）

	// ズームイン演出用
	bool isZoomingIn_ = false;
	float zoomInTimer_ = 0.0f;
	const float zoomInDuration_ = 0.6f; // ズームインにかける時間（秒）

	// ズーム後の待機時間用
	bool isWaitingAfterZoom_ = false;
	float zoomWaitTimer_ = 0.0f;
	const float zoomWaitDuration_ = 0.7f; // ズーム完了後の待機時間（秒）

	// UI用カメラ
	std::unique_ptr<Camera> uiCamera_ = nullptr;

	// UI用DrawResource
	std::unique_ptr<DrawResource> background_ = nullptr;
	std::unique_ptr<DrawResource> selectText_ = nullptr;

	// ステージプレビュー用DrawResource
	std::unique_ptr<DrawResource> stagePreview1_ = nullptr;
	std::unique_ptr<DrawResource> stagePreview2_ = nullptr;
	std::unique_ptr<DrawResource> stagePreview3_ = nullptr;

	// ステージ選択
	int selectedStageIndex_ = 0;  // 0: Level1, 1: Level2, 2: Level3
	int previousStageIndex_ = 0;  // 前回選択されていたステージ
	bool isStageSelected_ = false;  // スペースキーで決定されたかどうか

	// イージングアニメーション用
	struct StageAnimationData {
		Vector3 currentPosition;  // 現在の位置
		Vector3 targetPosition;   // 目標位置
		Vector3 currentScale;     // 現在のスケール
		Vector3 targetScale;      // 目標スケール
		Vector3 currentRotation;  // 現在の回転
		Vector3 targetRotation;   // 目標回転
		uint32_t currentColor;    // 現在のカラー
		uint32_t targetColor;     // 目標カラー
		float animationTimer;     // アニメーションタイマー
		bool isAnimating;         // アニメーション中かどうか
	};

	StageAnimationData stage1Animation_;
	StageAnimationData stage2Animation_;
	StageAnimationData stage3Animation_;

	const float stageAnimationDuration_ = 0.4f;  // ステージアニメーションの時間（秒）

	// ステージの位置定数（選択時は中央、非選択時は左右に配置）
	const Vector3 centerPosition_ = { 0.0f, 0.0f, 50.0f };       // 中央（選択時）
	const Vector3 leftPosition_ = { -700.0f, 0.0f, 50.0f };      // 左側（非選択時、画面外に近い）
	const Vector3 rightPosition_ = { 700.0f, 0.0f, 50.0f };      // 右側（非選択時、画面外に近）
	const Vector3 farLeftPosition_ = { -1400.0f, 0.0f, 50.0f };  // 最左（完全に画面外）
	const Vector3 farRightPosition_ = { 1400.0f, 0.0f, 50.0f };  // 最右（完全に画面外）

	// ズームイン演出用の目標値
	const Vector3 zoomScale_ = { 1280.0f, 720.0f, 1.0f };        // 画面いっぱいのスケール

	// ステージのスケールとカラーの定数
	const Vector3 selectedScale_ = { 600.0f, 337.5f, 1.0f };    // 選択時のスケール（大きめ）
	const Vector3 unselectedScale_ = { 400.0f, 225.0f, 1.0f };  // 非選択時のスケール
	const uint32_t selectedColor_ = 0xffffffff;                  // 選択時のカラー（明るい）
	const uint32_t unselectedColor_ = 0x808080ff;                // 非選択時のカラー（暗い）

	// テクスチャハンドル
	int backgroundTextureHandle_ = 0;
	int selectTextTextureHandle_ = 0;

};
		
