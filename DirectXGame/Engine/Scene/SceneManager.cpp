#include "SceneManager.h"
#include "../../Scene/Common/CommonData.h"
#include "../../Scene/TitleScene.h"
#include "../Input/Input.h"
#include "../Sound/Sound.h"

SceneManager::SceneManager(const int32_t kWindowWidth, const int32_t kWindowHeight) {

	commonData_ = new CommonData();

	//最初のシーンを挿入
	scene_ = new TitleScene(commonData_);

	nextScene_ = nullptr;

	myDirectX_ = new MyDirectX(kWindowWidth, kWindowHeight);

	renderer_ = new Render(myDirectX_);
	scene_->SetRenderer(renderer_);

	input_ = new Input(myDirectX_->GetMyWndClass().hInstance, myDirectX_->GetMyHwnd());
	input_->Initialize();

	sound_ = new Sound();
	//↓↓↓↓↓↓↓↓↓↓↓↓↓↓読み込みたい音↓↓↓↓↓↓↓↓↓↓↓↓↓

	//↑↑↑↑↑↑↑↑↑↑↑↑↑↑読み込みたい音↑↑↑↑↑↑↑↑↑↑↑↑↑



	//↓↓↓↓↓↓↓↓↓↓↓↓↓↓読み込みたい画像↓↓↓↓↓↓↓↓↓↓↓↓↓

	//↑↑↑↑↑↑↑↑↑↑↑↑↑↑読み込みたい画像↑↑↑↑↑↑↑↑↑↑↑↑↑

	//↓↓↓↓↓↓↓↓↓↓↓↓↓↓読み込みたい量↓↓↓↓↓↓↓↓↓↓↓↓↓
	myDirectX_->CreateDrawResource(MyDirectX::kPrism, 1);
	myDirectX_->CreateDrawResource(MyDirectX::kSphere, 1);
	myDirectX_->CreateDrawResource(MyDirectX::kBox, 1);
	//↑↑↑↑↑↑↑↑↑↑↑↑↑↑読み込みたい量↑↑↑↑↑↑↑↑↑↑↑↑↑
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

	myDirectX_->BeginFrame();

	input_->Update();

	if (nextScene_ != nullptr) {
		delete scene_;
		scene_ = nextScene_;
		nextScene_->SetRenderer(renderer_);
		nextScene_ = nullptr;
	}

	nextScene_ = scene_->Update();
}

void SceneManager::Draw() const{
	if (scene_ == nullptr) {
		assert(false && "SceneController::Render() Scene is nullptr.");
	}

	myDirectX_->PreDraw();
	scene_->Draw();
	myDirectX_->PostDraw();
}
