#pragma once
#include <Scene/Data/BaseScene.h>
#include <map>
#include <string>
#include <Scene/SceneManager.h>
#include <memory>

// 新しいクラスのインクルード
#include "SelectSceneUI/StageCarousel.h"
#include "SelectSceneUI/SelectSceneUI.h"
#include "SelectSceneUI/SelectSceneTransition.h"
#include "SelectSceneUI/SelectSceneInputHandler.h"
#include <Render/Resource/TimerResource.h>

class SelectScene : public BaseScene {
public:

	SelectScene();
	~SelectScene();

	void Initialize() override;
	std::unique_ptr<BaseScene> Update() override;
	void Draw() override;

private:

	/// @brief シーン遷移判定
	/// @return 次のシーンのポインタ（遷移しない場合はnullptr）
	std::unique_ptr<BaseScene> CheckSceneTransition();

	static inline bool firstLoad_ = false;

	std::map<int, std::string> stageMap_;
	std::unique_ptr<SceneManager> sceneManager_;

	// UI用カメラ
	std::unique_ptr<Camera> uiCamera_ = nullptr;

	// グラデーション背景（UIクラスに含まれない特殊要素）
	std::unique_ptr<DrawResource> gradientBackground_ = nullptr;

	// 各機能クラス
	std::unique_ptr<StageCarousel> stageCarousel_;
	std::unique_ptr<SelectSceneUI> ui_;
	std::unique_ptr<SelectSceneTransition> transition_;
	std::unique_ptr<SelectSceneInputHandler> inputHandler_;


	// ステージ選択の初期化用定数
	inline static const Vector3 kCenterPosition = { 0.0f, 0.0f, 50.0f };
};
