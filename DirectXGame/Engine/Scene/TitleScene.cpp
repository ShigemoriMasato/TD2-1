#include "TitleScene.h"

TitleScene::TitleScene(CommonData* commonData) : BaseScene(commonData) {
}

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {
	bunnyHandle_ = modelManager_->LoadModel("bunny");
	bunnyModel_ = std::make_unique<ModelResource>();
	bunnyModel_->Initialize(modelManager_->GetModelData(bunnyHandle_));
}

std::unique_ptr<BaseScene> TitleScene::Update() {
	return std::unique_ptr<BaseScene>();
}

void TitleScene::Draw() {
	render_->Draw(bunnyModel_.get());
}
