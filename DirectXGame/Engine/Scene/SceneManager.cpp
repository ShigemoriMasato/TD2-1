#include "SceneManager.h"
#include "../../Scene/Common/CommonData.h"
#include "../../Scene/TitleScene.h"
#include "../Input/Input.h"
#include "../Sound/Sound.h"
#include <random>
#include <time.h>

SceneManager::SceneManager(const int32_t kWindowWidth, const int32_t kWindowHeight) {

	commonData_ = std::make_shared<CommonData>();

	nextScene_ = nullptr;

	myDirectX_ = new MyDirectX(kWindowWidth, kWindowHeight);

	render_ = new Render(myDirectX_);

	input_ = new Input(myDirectX_->GetMyWndClass().hInstance, myDirectX_->GetMyHwnd());
	input_->Initialize();

	sound_ = new Sound();

	std::srand(uint32_t(time(nullptr)));

	//↓↓↓↓↓↓↓↓↓↓↓↓↓↓読み込みたい音↓↓↓↓↓↓↓↓↓↓↓↓↓

	//↑↑↑↑↑↑↑↑↑↑↑↑↑↑読み込みたい音↑↑↑↑↑↑↑↑↑↑↑↑↑

	//↓↓↓↓↓↓↓↓↓↓↓↓↓↓読み込みたい画像↓↓↓↓↓↓↓↓↓↓↓↓↓
	
	//↑↑↑↑↑↑↑↑↑↑↑↑↑↑読み込みたい画像↑↑↑↑↑↑↑↑↑↑↑↑↑

	//↓↓↓↓↓↓↓↓↓↓↓↓↓↓読み込みたいモデル↓↓↓↓↓↓↓↓↓↓↓↓↓
	
	//↑↑↑↑↑↑↑↑↑↑↑↑↑↑読み込みたいモデル↑↑↑↑↑↑↑↑↑↑↑↑↑

	//↓↓↓↓↓↓↓↓↓↓↓↓↓↓読み込みたい量↓↓↓↓↓↓↓↓↓↓↓↓↓
	myDirectX_->CreateDrawResource(MyDirectX::kSprite, 100);
	myDirectX_->CreateDrawResource(MyDirectX::kLine, 1000);
	//↑↑↑↑↑↑↑↑↑↑↑↑↑↑読み込みたい量↑↑↑↑↑↑↑↑↑↑↑↑↑

	//最初のシーンを挿入
	scene_ = new TitleScene(commonData_);
	scene_->Initialize();
}

SceneManager::~SceneManager() {
	delete input_;
	delete myDirectX_;
	delete render_;
	delete scene_;
	if (nextScene_) {
		delete nextScene_;
	}
}

void SceneManager::Update() {

	myDirectX_->BeginFrame();

	input_->Update();

	if (nextScene_ != nullptr) {
		delete scene_;
		scene_ = nextScene_;
		nextScene_ = nullptr;
		scene_->Initialize();
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
