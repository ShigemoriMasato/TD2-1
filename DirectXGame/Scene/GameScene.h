#pragma once
#include "Common/Scene.h"
#include "Common/CommonData.h"
#include "../Engine/Camera/DebugCamera.h"

class GameScene : public Scene {
public:
	GameScene(std::shared_ptr<CommonData> commonData);
	~GameScene();

	void Initialize() override;
	Scene* Update() override;
	void Draw() const override;

private:
	Camera* camera_ = nullptr;
	DebugCamera* debugCamera = nullptr;

	bool isDebugCamera = false;
};
