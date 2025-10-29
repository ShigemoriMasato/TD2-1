#pragma once
#include <Scene/Data/BaseScene.h>

class TitleScene : public BaseScene {
public:

	/// @brief 初期化
	void Initialize() override;

	/// @brief 更新処理
	/// @return シーン移動先のシーンポインタ。移動しない場合はnullptrを返す
	std::unique_ptr<BaseScene> Update() override;

	/// @brief 描画
	void Draw() override;

private:

	/// @brief フェード処理の更新
	void UpdateFade(float deltaTime);

	/// @brief グリッチエフェクトの更新
	void UpdateGlitch(float deltaTime);

	/// @brief タイトルロゴのアニメーション更新
	void UpdateTitleLogoAnimation(float deltaTime);

	/// @brief Space Startテキストのアニメーション更新
	void UpdateSpaceStartAnimation(float deltaTime);

	/// @brief イージング関数（EaseOutCubic）
	float EaseOutCubic(float t);

	/// @brief イージング関数（EaseInOutSine）
	float EaseInOutSine(float t);

	std::unique_ptr<PostEffectResource> postEffect_ = nullptr;

	// フェード処理用
	bool isFading_ = false;
	float fadeTimer_ = 0.0f;
	const float fadeDuration_ = 2.0f; // フェードにかける時間（秒）

	// グリッチエフェクト用
	float glitchTimer_ = 0.0f;
	float glitchInterval_ = 3.0f;      // グリッチ発生基本間隔（秒）- 短縮
	float glitchIntervalVariation_ = 2.0f;  // 間隔のランダム幅（秒）- 狭める
	float glitchDuration_ = 0.0f;      // 現在のグリッチ継続時間
	float glitchMaxDuration_ = 0.5f;   // グリッチの最大継続時間（秒）- デフォルトを長く
	bool isGlitching_ = false;         // グリッチ中かどうか
	float nextGlitchTime_ = 0.0f;      // 次のグリッチ発生時刻

	// UI用カメラ
	std::unique_ptr<Camera> uiCamera_ = nullptr;

	// UI用DrawResource
	std::unique_ptr<DrawResource> titleLogo_ = nullptr;
	std::unique_ptr<DrawResource> spaceStart_ = nullptr;
	std::unique_ptr<DrawResource> background_ = nullptr;

	// テクスチャハンドル
	int titleTextureHandle_ = 0;
	int spaceStartTextureHandle_ = 0;
	int backgroundTextureHandle_ = 0;

	// アニメーション用パラメータ
	float totalTime_ = 0.0f; // 総経過時間
	
	// タイトルロゴアニメーション
	float titleLogoAnimTime_ = 0.0f;
	const float titleLogoFadeDuration_ = 1.2f;  // フェードイン時間
	const float titleLogoSlideDistance_ = 100.0f; // スライド距離
	const float titleLogoFloatAmplitude_ = 8.0f;  // 浮遊振幅
	const float titleLogoFloatSpeed_ = 1.5f;      // 浮遊速度
	Vector3 titleLogoInitialPos_ = { 0.0f, 160.0f, 0.0f }; // 初期位置
	
	// Space Startアニメーション
	float spaceStartAnimTime_ = 0.0f;
	const float spaceStartDelayTime_ = 0.8f;     // 開始遅延
	const float spaceStartFadeDuration_ = 0.8f;  // フェードイン時間
	const float spaceStartPulseSpeed_ = 2.0f;    // パルス速度
	const float spaceStartPulseMin_ = 0.6f;      // パルス最小アルファ
	const float spaceStartPulseMax_ = 1.0f;      // パルス最大アルファ

};

