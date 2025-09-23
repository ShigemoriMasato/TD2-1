#include "SceneManager.h"
#include "TitleScene.h"

SceneManager::SceneManager() {
	engineTerminal_ = std::make_unique<EngineTerminal>();
	engineTerminal_->Initialize(1280, 720);

	commonData_ = std::make_shared<CommonData>();
	currentScene_ = std::make_unique<TitleScene>(commonData_.get());
}

SceneManager::~SceneManager() {
}

bool SceneManager::IsLoop() {
	return engineTerminal_->IsLoop();
}

void SceneManager::Update() {

}

void SceneManager::Draw() {
	
}
