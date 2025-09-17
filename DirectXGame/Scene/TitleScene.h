#pragma once
#include "Common/Scene.h"

class TitleScene : public Scene {
public:

	TitleScene(std::shared_ptr<CommonData> commonData);
	~TitleScene();
	void Initialize() override;
	std::unique_ptr<Scene> Update() override;
	void Draw() const override;

private:
	Camera* camera_ = nullptr;
};
