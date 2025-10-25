#pragma once
#include <Scene/Data/BaseScene.h>

class SelectScene : public BaseScene {
public:

	SelectScene();
	~SelectScene();

	void Initialize() override;
	std::unique_ptr<BaseScene> Update() override;
	void Draw() override;

private:



};
