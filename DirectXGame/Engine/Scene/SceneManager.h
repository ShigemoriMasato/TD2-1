#pragma once
#include <memory>
#include "Data/BaseScene.h"

class SceneManager {
public:

	SceneManager();
	~SceneManager();

	void Initialize();

	bool IsLoop();
	void Update();
	void Draw();
	
private:

	std::unique_ptr<EngineTerminal> engineTerminal_ = nullptr;
	std::shared_ptr<CommonData> commonData_ = nullptr;
	std::unique_ptr<BaseScene> currentScene_ = nullptr;
	std::unique_ptr<BaseScene> nextScene_ = nullptr;

};

