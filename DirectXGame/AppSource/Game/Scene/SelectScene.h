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

	/// @brief シーン遷移判定
	/// @return 次のシーンのポインタ（遷移しない場合はnullptr）
	std::unique_ptr<BaseScene> CheckSceneTransition();

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
	bool isStageSelected_ = false;  // スペースキーで決定されたかどうか

	// テクスチャハンドル
	int backgroundTextureHandle_ = 0;
	int selectTextTextureHandle_ = 0;

};
