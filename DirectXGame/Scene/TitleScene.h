#pragma once
#include "Common/Scene.h"
#include "Common/CommonData.h"
#include "../Engine/Camera/DebugCamera.h"
#include "../Player.h"

class TitleScene : public Scene {
public:
	TitleScene(CommonData* commonData);
	~TitleScene();

	Scene* Update() override;
	void Draw() const override;

private:

	Player* player_ = nullptr;

	Camera* camera;
	DebugCamera* debugCamera;

	bool isDebugCamera = false;

};
