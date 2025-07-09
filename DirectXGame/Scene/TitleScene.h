#pragma once
#include "Common/Scene.h"
#include "../Engine/Camera/DebugCamera.h"
#include <array>
#include "../GridMaker.h"

class TitleScene : public Scene {
public:

	TitleScene(std::shared_ptr<CommonData> commonData);
	~TitleScene();
	void Initialize() override;
	Scene* Update() override;
	void Draw() const override;

private:
	Camera* camera_ = nullptr;
	DebugCamera* debugCamera_;
	Transform cameraTransform_;

	GridMaker* gridMaker_;
};
