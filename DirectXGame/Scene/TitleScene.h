#pragma once
#include "Common/Scene.h"
#include "../TitleLogo.h"

class TitleScene : public Scene {
public:

	TitleScene(std::shared_ptr<CommonData> commonData);
	~TitleScene();
	void Initialize() override;
	std::unique_ptr<Scene> Update() override;
	void Draw() const override;

private:
	std::shared_ptr<Camera> camera_ = nullptr;
	std::unique_ptr<TitleLogo> titleLogo_ = nullptr;
};
