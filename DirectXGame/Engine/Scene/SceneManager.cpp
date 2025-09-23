#include "SceneManager.h"
#include "TitleScene.h"

SceneManager::SceneManager() {
	engineTerminal_ = std::make_unique<EngineTerminal>();
	commonData_ = std::make_shared<CommonData>();
	currentScene_ = std::make_unique<TitleScene>(commonData_.get());
}

SceneManager::~SceneManager() {
}

bool SceneManager::IsLoop() {

	

	return true;
}

void SceneManager::Update() {

	if (nextScene_) {
		currentScene_ = std::move(nextScene_);
		currentScene_->MoveScene(engineTerminal_.get());
		currentScene_->Initialize();
		nextScene_ = nullptr;
	}

	nextScene_ = currentScene_->Update();

}

void SceneManager::Draw() {
	engineTerminal_->GetRender()->PreDraw();
	currentScene_->Draw();
	engineTerminal_->GetRender()->PostDraw();
}
