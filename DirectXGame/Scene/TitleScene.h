#pragma once
#include "Common/Scene.h"
#include "../UI.h"

class TitleScene : public Scene {
public:

	TitleScene(std::shared_ptr<CommonData> commonData);
	~TitleScene();
	void Initialize() override;
	std::unique_ptr<Scene> Update() override;
	void Draw() const override;

private:

	Camera* camera_ = nullptr;
	Transform* cameraTrans_ = new Transform{ {0.0f, 0.0f, -5.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f} };

	UI* ui_;
};
