#include "TitleScene.h"

TitleScene::TitleScene() : BaseScene() {
}

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {
	camera_ = std::make_unique<DebugCamera>();
	camera_->Initialize();

	for (int i = 0; i < 1; ++i) {
		auto grid = std::make_unique<GridMaker>(camera_.get(), false);
		grid->Initialize();
		gridMaker_.push_back(std::move(grid));
	}

	bunnyHandle_ = modelManager_->LoadModel("DefaultDesc");
	bunnyModel_ = std::make_unique<ModelResource>();
	bunnyModel_->Initialize(modelManager_->GetModelData(bunnyHandle_));
	bunnyModel_->SetCamera(camera_.get());
	bunnyModel_->psoConfig_.isSwapChain = true;

	triangle_ = std::make_unique<ParticleResource>();
	triangle_->Initialize(3, 0, 10);
	triangle_->localPos_ = { {0.0f,1.0f,0.0f},{1.0f,-1.0f,0.0f},{-1.0f,-1.0f,0.0f} };
	triangle_->psoConfig_.isSwapChain = true;
	triangle_->camera_ = camera_.get();

	testEmitter_ = std::make_unique<DefaultEmitter>(1000);
	testEmitter_->Initialize(camera_.get());
}

std::unique_ptr<BaseScene> TitleScene::Update() {
	camera_->Update();

	for (auto& grid : gridMaker_) {
		grid->Update();
	}

	for (int i = 0; i < 10; ++i) {
		triangle_->position_[i] = { (float)i - 5.0f, 0.0f, 0.0f };
	}

	camera_->DrawImGui();
	camera_->MakeMatrix();

	testEmitter_->Update();
	testEmitter_->DrawImGui();

	return std::unique_ptr<BaseScene>();
}

void TitleScene::Draw() {
	//swapchainに描画
	render_->PreDraw();

	for(auto& grid : gridMaker_) {
		grid->Draw(render_);
	}
	render_->Draw(bunnyModel_.get());

	testEmitter_->Draw(render_);
}
