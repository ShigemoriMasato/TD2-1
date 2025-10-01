#pragma once
#include "Data/BaseScene.h"
#include <Render/Resource/ModelResource.h>
#include <Camera/DebugCamera.h>
#include <Tools/Grid/GridMaker.h>
#include <Emitter/TestEmitter.h>

class TitleScene : public BaseScene {
public:

	TitleScene();
	~TitleScene() override;

	void Initialize() override;
	std::unique_ptr<BaseScene> Update() override;
	void Draw() override;

private:

	int bunnyHandle_ = -1;
	std::unique_ptr<ModelResource> bunnyModel_ = nullptr;
	std::unique_ptr<DebugCamera> camera_ = nullptr;

	std::list<std::unique_ptr<GridMaker>> gridMaker_;
	std::unique_ptr<ParticleResource> triangle_ = nullptr;
	std::unique_ptr<TestEmitter> testEmitter_ = nullptr;
};
