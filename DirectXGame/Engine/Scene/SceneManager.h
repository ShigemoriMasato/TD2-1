#pragma once
#include <memory>

class BaseScene;
class EngineTerminal;
class CommonData;

class SceneManager {
public:

	SceneManager(std::unique_ptr<BaseScene> firstScene, EngineTerminal* engine);
	~SceneManager();

	void Update();
	void Draw();

	std::shared_ptr<CommonData> commonData_ = nullptr;

private:

	EngineTerminal* engineTerminal_ = nullptr;
	std::unique_ptr<BaseScene> currentScene_ = nullptr;
	std::unique_ptr<BaseScene> nextScene_ = nullptr;

};

