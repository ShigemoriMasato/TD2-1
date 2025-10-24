#pragma once
#include <Scene/Data/BaseScene.h>
#include <map>
#include <string>

class SelectScene : public BaseScene {
public:

	SelectScene();
	~SelectScene();

	void Initialize() override;
	std::unique_ptr<BaseScene> Update() override;
	void Draw() override;

private:

	void CreateStageTexture();

	static inline bool firstLoad_ = false;

	std::map<int, std::string> stageMap_;

};
