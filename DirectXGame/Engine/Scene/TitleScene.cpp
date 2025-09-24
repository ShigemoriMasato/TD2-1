#include "TitleScene.h"

TitleScene::TitleScene(CommonData* commonData) : BaseScene(commonData) {
}

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {
	camera_ = std::make_unique<DebugCamera>();
	camera_->Initialize();
}

std::unique_ptr<BaseScene> TitleScene::Update() {
	camera_->Update();

	return std::unique_ptr<BaseScene>();
}

void TitleScene::Draw() {
}
