#pragma once
#include <Render/Resource/DrawResource.h>
#include <Resource/OffScreen/OffScreenManager.h>
#include <Transform/Vector.h>
#include <memory>
#include <vector>

// 前方宣言
class Camera;

/// <summary>
/// ステージ選択カルーセル
/// ステージプレビューの配置、アニメーション、選択状態の管理を担当
/// </summary>
class StageCarousel {
public:
	StageCarousel();
	~StageCarousel();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="camera">UI用カメラ</param>
	/// <param name="offScreenIndices">各ステージのオフスクリーンテクスチャインデックス配列（ポインタ渡し）</param>
	/// <param name="offScreenManager">オフスクリーンマネージャー</param>
	void Initialize(Camera* camera, const OffScreenIndex* offScreenIndices, OffScreenManager* offScreenManager);

	/// <summary>
	/// アニメーション更新
	/// </summary>
	/// <param name="deltaTime">デルタタイム</param>
	/// <param name="skipAnimation">アニメーションをスキップするか（ズームイン中など）</param>
	void Update(float deltaTime, bool skipAnimation = false);

	/// <summary>
	/// 選択インデックスを変更
	/// </summary>
	/// <param name="newIndex">新しい選択インデックス</param>
	void SetSelectedStageIndex(int newIndex);

	/// <summary>
	/// 選択インデックスを取得
	/// </summary>
	int GetSelectedStageIndex() const { return selectedStageIndex_; }

	/// <summary>
	/// ステージプレビューを取得
	/// </summary>
	const std::vector<std::unique_ptr<DrawResource>>& GetStagePreviews() const { return stagePreviews_; }

	/// <summary>
	/// 指定インデックスのステージプレビューを取得
	/// </summary>
	DrawResource* GetStagePreview(int index);

	/// <summary>
	/// ズームイン演出用にスケールと位置を更新
	/// </summary>
	/// <param name="stageIndex">対象ステージインデックス</param>
	/// <param name="scale">スケール</param>
	/// <param name="brightness">明るさ（色の倍率）</param>
	void UpdateZoomEffect(int stageIndex, const Vector3& scale, float brightness);

	/// <summary>
	/// 他のステージを画面外に移動
	/// </summary>
	/// <param name="selectedIndex">選択されているステージ</param>
	/// <param name="offset">オフセット量</param>
	void MoveOtherStagesOffScreen(int selectedIndex, float offset);

private:
	/// <summary>
	/// ステージプレビューの目標値を設定してアニメーション開始
	/// </summary>
	void SetStagePreviewTargets();

	/// <summary>
	/// イージング関数（EaseOutCubic）
	/// </summary>
	float EaseOutCubic(float t);

	// ステージアニメーションデータ
	struct StageAnimationData {
		Vector3 currentPosition;
		Vector3 targetPosition;
		Vector3 currentScale;
		Vector3 targetScale;
		uint32_t currentColor;
		uint32_t targetColor;
		float animationTimer;
		bool isAnimating;
	};

	// ステージプレビュー用DrawResource（5個）
	std::vector<std::unique_ptr<DrawResource>> stagePreviews_;

	// アニメーションデータ（5個）
	std::vector<StageAnimationData> stageAnimations_;

	// 選択ステージインデックス
	int selectedStageIndex_ = 0;
	int previousStageIndex_ = 0;

	// アニメーション設定
	static constexpr float kAnimationDuration = 0.4f;

	// 位置定数（inline constを使用してヘッダー内定義）
	inline static const Vector3 kCenterPosition = { 0.0f, 0.0f, 50.0f };
	inline static const Vector3 kLeftPosition = { -700.0f, 0.0f, 50.0f };
	inline static const Vector3 kRightPosition = { 700.0f, 0.0f, 50.0f };
	inline static const Vector3 kFarLeftPosition = { -1400.0f, 0.0f, 50.0f };
	inline static const Vector3 kFarRightPosition = { 1400.0f, 0.0f, 50.0f };

	// スケールとカラー定数
	inline static const Vector3 kSelectedScale = { 600.0f, 337.5f, 1.0f };
	inline static const Vector3 kUnselectedScale = { 400.0f, 225.0f, 1.0f };
	static constexpr uint32_t kSelectedColor = 0xffffffff;
	static constexpr uint32_t kUnselectedColor = 0x606060ff;
};
