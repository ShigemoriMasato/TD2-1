#include "TitleScene.h"

TitleScene::TitleScene() : BaseScene() {
}

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {
	camera_ = std::make_unique<DebugCamera>();
	camera_->Initialize();

	for (int i = 0; i < 1; ++i) {
		auto grid = std::make_unique<GridMaker>(camera_.get(), true);
		grid->Initialize();
		gridMaker_.push_back(std::move(grid));
	}

	bunnyHandle_ = modelManager_->LoadModel("DefaultDesc");
	bunnyModel_ = std::make_unique<ModelResource>();
	bunnyModel_->Initialize(modelManager_->GetModelData(bunnyHandle_));
	bunnyModel_->SetCamera(camera_.get());
	bunnyModel_->psoConfig_.isSwapChain = true;
}

std::unique_ptr<BaseScene> TitleScene::Update() {
	camera_->Update();

	for (auto& grid : gridMaker_) {
		grid->Update();
	}

	return std::unique_ptr<BaseScene>();
}

void TitleScene::Draw() {
	//swapchainに描画
	render_->PreDraw();

	for(auto& grid : gridMaker_) {
		grid->Draw(render_);
	}
	render_->Draw(bunnyModel_.get());
}
