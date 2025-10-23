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

};

