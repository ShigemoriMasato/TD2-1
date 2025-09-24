#include "SceneManager.h"
#include "TitleScene.h"

SceneManager::SceneManager() {
	engineTerminal_ = std::make_unique<EngineTerminal>();
	engineTerminal_->Initialize(1280, 720);

	commonData_ = std::make_shared<CommonData>();
	currentScene_ = std::make_unique<TitleScene>(commonData_.get());
	currentScene_->MoveScene(engineTerminal_.get());
	currentScene_->Initialize();
}

SceneManager::~SceneManager() {
}

bool SceneManager::IsLoop() {
	return engineTerminal_->IsLoop();
}

void SceneManager::Update() {

	engineTerminal_->Update();

	if (nextScene_) {
		nextScene_->MoveScene(engineTerminal_.get());
		currentScene_ = std::move(nextScene_);
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
