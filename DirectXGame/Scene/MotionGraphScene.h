#pragma once
#include "Common/Scene.h"
#include "../Object/Actor.h"
#include "../SampleGraph.h"

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
	std::unique_ptr<SampleGraph> sampleGraph_ = nullptr;

};

