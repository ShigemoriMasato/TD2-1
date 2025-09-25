#include "TitleScene.h"

TitleScene::TitleScene(CommonData* commonData) : BaseScene(commonData) {
}

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {
	camera_ = std::make_unique<DebugCamera>();
	camera_->Initialize();

	gridMaker_ = std::make_unique<GridMaker>(camera_.get(), true);
	gridMaker_->Initialize();

	bunnyHandle_ = modelManager_->LoadModel("bunny");
	bunnyModel_ = std::make_unique<ModelResource>();
	bunnyModel_->Initialize(modelManager_->GetModelData(bunnyHandle_));
	bunnyModel_->SetCamera(camera_.get());
	bunnyModel_->psoConfig_.isSwapChain = true;
}

std::unique_ptr<BaseScene> TitleScene::Update() {
	camera_->Update();

	gridMaker_->Update();

	return std::unique_ptr<BaseScene>();
}

void TitleScene::Draw() {
	gridMaker_->Draw(render_);
	render_->Draw(bunnyModel_.get());
}
