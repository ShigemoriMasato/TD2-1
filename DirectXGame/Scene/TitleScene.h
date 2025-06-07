#pragma once
#include "Common/Scene.h"
#include "Common/CommonData.h"
#include "../Tools/Camera.h"

class TitleScene : public Scene {
public:
	TitleScene(CommonData* commonData);
	~TitleScene();

	Scene* Update() override;
	void Render() override;

private:

	Camera* camera;

};
