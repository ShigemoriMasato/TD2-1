#include "TitleScene.h"
#include <imgui/ImGuizmo.h>

TitleScene::TitleScene() : BaseScene() {
}

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {
	camera_ = std::make_unique<DebugCamera>();
	camera_->Initialize();

	imguiWrapper_ = std::make_unique<ImGuiWrapper>();

	for (int i = 0; i < 1; ++i) {
		auto grid = std::make_unique<GridMaker>(camera_.get());
		grid->Initialize();
		gridMaker_.push_back(std::move(grid));
	}

	descHandle_ = modelManager_->LoadModel("walk");
	descModel_ = std::make_unique<ModelResource>();
	descModel_->Initialize(modelManager_, descHandle_);
	descModel_->camera_ = camera_.get();
	descTransform_ = {};
	descTransform_.scale = { 1.0f, 1.0f, 1.0f };
	descModel_->psoConfig_.isSwapChain = true;
	imguiWrapper_->AddItem("Desc", &worldMatrix_, &descTransform_);

	skel_ = std::make_unique<SkeletonDrawer>();
	skel_->Initialize(modelManager_->GetModelData(descHandle_)->GetSkeleton(), modelManager_->GetAnimation(descHandle_), camera_.get());

	testEmitter_ = std::make_unique<DefaultEmitter>(1000);
	testEmitter_->Initialize(camera_.get());

	imguiWrapper_->SetCamera(camera_.get());

	wavHandle = audio_->Load("fanfare.wav");
	mp3Handle = audio_->Load("Clear.mp3");
}

std::unique_ptr<BaseScene> TitleScene::Update() {
	keys_ = commonData->keyManager_->GetKeyStates();

	camera_->Update();

	for (auto& grid : gridMaker_) {
		grid->Update();
	}

	camera_->MakeMatrix();
	testEmitter_->Update();

	// =====================- Audioテスト -===========================
	ImGui::Begin("Audio");
	if (ImGui::Button("WAV")) {
		audio_->Play(wavHandle, false);
	}
	if (ImGui::Button("MP3")) {
		audio_->Play(mp3Handle, false);
	}
	ImGui::End();

	skel_->Update();

	// ====================- GuIzmoテスト -============================
	imguiWrapper_->GuizmoUpdate();
	descModel_->position_ = descTransform_.position;
	descModel_->rotate_ = descTransform_.rotation;
	descModel_->scale_ = descTransform_.scale;

	// ====================- Input系のテスト -==========================
	ImGui::Begin("Input");
	for(int i = 0; i < int(Key::Count); ++i) {
		ImGui::Text("%d : %d", i, keys_[Key(i)]);
	}
	ImGui::End();

	return std::unique_ptr<BaseScene>();
}

void TitleScene::Draw() {
	//swapchainに描画
	render_->PreDraw();

	for(auto& grid : gridMaker_) {
		grid->Draw(render_);
	}

	//render_->Draw(descModel_.get());

	skel_->Draw(render_);

	//testEmitter_->Draw(render_);
}
