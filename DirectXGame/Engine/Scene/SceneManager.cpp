#include "SceneManager.h"
#include "../../Scene/Common/CommonData.h"
#include "../../Scene/TitleScene.h"
#include "../../Scene/GameScene.h"
#include "../Input/Input.h"
#include "../Sound/Sound.h"
#include <random>
#include <time.h>
#include <fftw3.h>

SceneManager::SceneManager(const int32_t kWindowWidth, const int32_t kWindowHeight) {

	commonData_ = std::make_shared<CommonData>();

	nextScene_ = nullptr;

	myDirectX_ = std::make_shared<MyDirectX>(kWindowWidth, kWindowHeight);

	render_ = std::make_unique<Render>(myDirectX_.get());

	input_ = std::make_unique<Input>(myDirectX_->GetMyWndClass().hInstance, myDirectX_->GetMyHwnd());
	input_->Initialize();

	sound_ = std::make_unique<Sound>();

	std::srand(uint32_t(time(nullptr)));

	//↓↓↓↓↓↓↓↓↓↓↓↓↓↓読み込みたい音↓↓↓↓↓↓↓↓↓↓↓↓↓
	sound_->LoadAudio("resources/TitleBGM.wav");
	sound_->LoadAudio("resources/fanfare.wav");
	//↑↑↑↑↑↑↑↑↑↑↑↑↑↑読み込みたい音↑↑↑↑↑↑↑↑↑↑↑↑↑

	commonData_->textureHandle_.resize(int(TextureType::TextureCount));
	//↓↓↓↓↓↓↓↓↓↓↓↓↓↓読み込みたい画像↓↓↓↓↓↓↓↓↓↓↓↓↓
	
	//↑↑↑↑↑↑↑↑↑↑↑↑↑↑読み込みたい画像↑↑↑↑↑↑↑↑↑↑↑↑↑

	commonData_->modelHandle_.resize(int(ModelType::ModelCount));
	//↓↓↓↓↓↓↓↓↓↓↓↓↓↓読み込みたいモデル↓↓↓↓↓↓↓↓↓↓↓↓↓
	//↑↑↑↑↑↑↑↑↑↑↑↑↑↑読み込みたいモデル↑↑↑↑↑↑↑↑↑↑↑↑↑

	//↓↓↓↓↓↓↓↓↓↓↓↓↓↓描画したい量↓↓↓↓↓↓↓↓↓↓↓↓↓
	myDirectX_->CreateDrawResource(MyDirectX::kSprite, 2000);
	myDirectX_->CreateDrawResource(MyDirectX::kLine, 100);
	//↑↑↑↑↑↑↑↑↑↑↑↑↑↑描画したい量↑↑↑↑↑↑↑↑↑↑↑↑↑

	//最初のシーンを挿入
	scene_ = std::make_unique<TitleScene>(commonData_);
	scene_->Initialize();

}

SceneManager::~SceneManager() {
}

bool SceneManager::IsRoop() {

	while (msg.message != WM_QUIT) {

		//メッセージがあれば処理する
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			//メッセージがなければ処理を始める
			return true;
		}

	}

	//ウィンドウのxボタンが押されたらfalseを返す
	return false;
}

void SceneManager::Update() {

	myDirectX_->BeginFrame();

	input_->Update();

	if (nextScene_ != nullptr && scene_.get() != nextScene_.get()) {
		scene_.reset();
		scene_ = std::move(nextScene_);
		scene_->Initialize();
	}

	nextScene_ = std::move(scene_->Update());
}

void SceneManager::Draw() const{
	if (scene_ == nullptr) {
		assert(false && "SceneController::Render() Scene is nullptr.");
	}

	myDirectX_->PreDraw();
	scene_->Draw();
	myDirectX_->PostDraw();

	sound_->PlayAudio();
}
