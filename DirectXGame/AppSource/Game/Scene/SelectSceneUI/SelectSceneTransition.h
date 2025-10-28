#pragma once
#include <Render/Resource/PostEffectResource.h>
#include <Transform/Vector.h>
#include <memory>

// 前方宣言
class StageCarousel;

/// <summary>
/// セレクトシーンのトランジション効果管理
/// フェードイン/アウト、ズームイン演出を担当
/// </summary>
class SelectSceneTransition {
public:
	SelectSceneTransition();
	~SelectSceneTransition();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// フェードイン更新
	/// </summary>
	/// <param name="deltaTime">デルタタイム</param>
	/// <returns>フェードイン完了したらtrue</returns>
	bool UpdateFadeIn(float deltaTime);

	/// <summary>
	/// フェードアウト更新
	/// </summary>
	/// <param name="deltaTime">デルタタイム</param>
	/// <returns>フェードアウト完了したらtrue</returns>
	bool UpdateFadeOut(float deltaTime);

	/// <summary>
	/// ズームイン演出更新
	/// </summary>
	/// <param name="deltaTime">デルタタイム</param>
	/// <param name="carousel">ステージカルーセル</param>
	/// <param name="selectedStageIndex">選択されているステージインデックス</param>
	/// <returns>ズームイン完了したらtrue</returns>
	bool UpdateZoomIn(float deltaTime, StageCarousel* carousel, int selectedStageIndex);

	/// <summary>
	/// ズーム後待機時間更新
	/// </summary>
	/// <param name="deltaTime">デルタタイム</param>
	/// <param name="carousel">ステージカルーセル</param>
	/// <param name="selectedStageIndex">選択されているステージインデックス</param>
	/// <returns>待機完了したらtrue</returns>
	bool UpdateZoomWait(float deltaTime, StageCarousel* carousel, int selectedStageIndex);

	/// <summary>
	/// ズームイン演出を開始
	/// </summary>
	void StartZoomIn();

	/// <summary>
	/// フェードアウトを開始
	/// </summary>
	void StartFadeOut();

	/// <summary>
	/// フェードイン中かどうか
	/// </summary>
	bool IsFadingIn() const { return isFadingIn_; }

	/// <summary>
	/// フェードアウト中かどうか
	/// </summary>
	bool IsFadingOut() const { return isFadingOut_; }

	/// <summary>
	/// フェードアウトが完了したかどうか
	/// </summary>
	bool IsFadeOutComplete() const { return fadeOutTimer_ >= kFadeOutDuration && isFadingOut_; }

	/// <summary>
	/// ズームイン中かどうか
	/// </summary>
	bool IsZoomingIn() const { return isZoomingIn_; }

	/// <summary>
	/// ズーム後待機中かどうか
	/// </summary>
	bool IsWaitingAfterZoom() const { return isWaitingAfterZoom_; }

	/// <summary>
	/// 背景グリッドアニメーション更新（ステージ選択変更時の演出）
	/// </summary>
	/// <param name="deltaTime">デルタタイム</param>
	void UpdateBackgroundGrid(float deltaTime);

	/// <summary>
	/// 背景グリッドアニメーションを開始
	/// </summary>
	void TriggerBackgroundGrid();

	/// <summary>
	/// 背景グリッドアニメーション中かどうか
	/// </summary>
	bool IsBackgroundGridAnimating() const { return isBackgroundGridAnimating_; }

	/// <summary>
	/// ポストエフェクトリソースを取得
	/// </summary>
	PostEffectResource* GetPostEffect() const { return postEffect_.get(); }

private:
	/// <summary>
	/// イージング関数（EaseInCubic）
	/// </summary>
	float EaseInCubic(float t);

	/// <summary>
	/// イージング関数（EaseOutCubic）
	/// </summary>
	float EaseOutCubic(float t);

	// ポストエフェクト
	std::unique_ptr<PostEffectResource> postEffect_;

	// フェードイン処理用
	bool isFadingIn_ = true;
	float fadeInTimer_ = 0.0f;
	static constexpr float kFadeInDuration = 1.5f;

	// フェードアウト処理用
	bool isFadingOut_ = false;
	float fadeOutTimer_ = 0.0f;
	static constexpr float kFadeOutDuration = 2.0f;

	// ズームイン演出用
	bool isZoomingIn_ = false;
	float zoomInTimer_ = 0.0f;
	static constexpr float kZoomInDuration = 0.6f;

	// ズーム後の待機時間用
	bool isWaitingAfterZoom_ = false;
	float zoomWaitTimer_ = 0.0f;
	static constexpr float kZoomWaitDuration = 0.8f;

	// ズームイン時のスケール
	inline static const Vector3 kZoomScale = { 1280.0f, 720.0f, 1.0f };

	// 背景グリッドアニメーション用
	bool isBackgroundGridAnimating_ = false;
	float backgroundGridTimer_ = 0.0f;
	static constexpr float kBackgroundGridDuration = 0.8f;  // アニメーション時間
	static constexpr float kBackgroundGridIntensity = 0.3f; // 最大の進行度（控えめ）
};
