#include "SceneManager.h"
#include "../Scene/Common/CommonData.h"
#include "../Scene/TitleScene.h"
#include "../Tools/Input.h"
#include "Core/Renderer.h"

SceneManager::SceneManager(const int32_t kWindowWidth, const int32_t kWindowHeight) {

	commonData_ = new CommonData();

	//最初のシーンを挿入
	scene_ = new TitleScene(commonData_);

	nextScene_ = nullptr;

	myDirectX_ = new MyDirectX(kWindowWidth, kWindowHeight);

	renderer_ = new Renderer(myDirectX_);
	scene_->SetRenderer(renderer_);

	input_ = new Input(myDirectX_->GetMyWndClass().hInstance, myDirectX_->GetMyHwnd());
	input_->Initialize();

	myDirectX_->CreateDrawResource(MyDirectX::kTriangle, 1);
}

SceneManager::~SceneManager() {
	delete input_;
	delete myDirectX_;
	delete renderer_;
	delete scene_;
	if (nextScene_) {
		delete nextScene_;
	}
	delete commonData_;
}

void SceneManager::Update() {

	input_->Update();

	if (nextScene_ != nullptr) {
		delete scene_;
		scene_ = nextScene_;
		nextScene_->SetRenderer(renderer_);
		nextScene_ = nullptr;
	}

	nextScene_ = scene_->Update();
}

void SceneManager::Render() {
	if (scene_ == nullptr) {
		assert(false && "SceneController::Render() Scene is nullptr.");
	}

	myDirectX_->BeginFrame();
	scene_->Render();
	myDirectX_->EndFrame();
}
