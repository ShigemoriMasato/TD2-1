#pragma once
#include "Data/BaseScene.h"
#include <Render/Resource/ModelResource.h>

class TitleScene : public BaseScene {
public:

	TitleScene(CommonData* commonData);
	~TitleScene() override;

	void Initialize() override;
	std::unique_ptr<BaseScene> Update() override;
	void Draw() override;

private:

	int bunnyHandle_ = -1;
	std::unique_ptr<ModelResource> bunnyModel_ = nullptr;

};
