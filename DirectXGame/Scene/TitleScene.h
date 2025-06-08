#pragma once
#include "Common/Scene.h"
#include "Common/CommonData.h"
#include "../Tools/Camera.h"
#include "../Tools/DebugCamera.h"

class TitleScene : public Scene {
public:
	TitleScene(CommonData* commonData);
	~TitleScene();

	Scene* Update() override;
	void Draw() const override;

private:

	Camera* camera;
	DebugCamera* debugCamera;

	bool isDebugCamera = false;

};
