#pragma once
#include "Data/BaseScene.h"

class TitleScene : public BaseScene {
public:

	TitleScene(CommonData* commonData);
	~TitleScene() override;

	void Initialize() override;
	std::unique_ptr<BaseScene> Update() override;
	void Draw() override;

private:



};
