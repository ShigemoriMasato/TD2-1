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

	std::unique_ptr<PostEffectResource> postEffect_ = nullptr;

	// フェード処理用
	bool isFading_ = false;
	float fadeTimer_ = 0.0f;
	const float fadeDuration_ = 1.0f; // フェードにかける時間（秒）

	// UI用カメラ(どうやっても出来なかった為カメラクラスを使用)
	std::unique_ptr<Camera> uiCamera_ = nullptr;

	// UI用DrawResource
	std::unique_ptr<DrawResource> titleLogo_ = nullptr;
	std::unique_ptr<DrawResource> spaceStart_ = nullptr;

	// テクスチャハンドル
	int titleTextureHandle_ = 0;
	int spaceStartTextureHandle_ = 0;

};

