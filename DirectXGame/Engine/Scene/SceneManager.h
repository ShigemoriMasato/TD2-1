#pragma once
#include <memory>

class BaseScene;
class EngineTerminal;
class CommonData;

class SceneManager {
public:

	SceneManager(std::unique_ptr<BaseScene> firstScene, EngineTerminal* engine);
	~SceneManager();

	bool IsLoop();
	void Update();
	void Draw();
	
private:

	EngineTerminal* engineTerminal_ = nullptr;
	std::shared_ptr<CommonData> commonData_ = nullptr;
	std::unique_ptr<BaseScene> currentScene_ = nullptr;
	std::unique_ptr<BaseScene> nextScene_ = nullptr;

};

