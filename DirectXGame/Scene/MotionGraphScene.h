#pragma once
#include "Common/Scene.h"

class MotionGraphScene : public Scene {
public:

	MotionGraphScene(std::shared_ptr<CommonData> commonData);
	~MotionGraphScene();

	void Initialize() override;
	std::unique_ptr<Scene> Update() override;
	void Draw() const override;

private:

	std::shared_ptr<Camera> camera_ = nullptr;
	Transform cameraTrans_;
};

