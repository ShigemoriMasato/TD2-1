#include "SceneManager.h"
#include "../../Scene/Common/CommonData.h"
#include "../../Scene/TitleScene.h"
#include "../../Scene/GameScene.h"
#include "../Input/Input.h"
#include "../Sound/Sound.h"
#include <random>
#include <time.h>

SceneManager::SceneManager(const int32_t kWindowWidth, const int32_t kWindowHeight) {

	myDirectX_ = std::make_shared<MyDirectX>(kWindowWidth, kWindowHeight);

	sound_ = std::make_unique<Sound>();

	input_ = std::make_unique<Input>(myDirectX_->GetMyWndClass().hInstance, myDirectX_->GetMyHwnd());
	input_->Initialize();

	commonData_ = std::make_shared<CommonData>();

	nextScene_ = nullptr;

	render_ = std::make_unique<Render>(myDirectX_.get());

	std::srand(uint32_t(time(nullptr)));

	//↓↓↓↓↓↓↓↓↓↓↓↓↓↓読み込みたい音↓↓↓↓↓↓↓↓↓↓↓↓↓

	//↑↑↑↑↑↑↑↑↑↑↑↑↑↑読み込みたい音↑↑↑↑↑↑↑↑↑↑↑↑↑

	commonData_->textureHandle_.resize(int(TextureType::TextureCount));
	//↓↓↓↓↓↓↓↓↓↓↓↓↓↓読み込みたい画像↓↓↓↓↓↓↓↓↓↓↓↓↓

	//↑↑↑↑↑↑↑↑↑↑↑↑↑↑読み込みたい画像↑↑↑↑↑↑↑↑↑↑↑↑↑

	commonData_->modelHandle_.resize(int(ModelType::ModelCount));
	//↓↓↓↓↓↓↓↓↓↓↓↓↓↓読み込みたいモデル↓↓↓↓↓↓↓↓↓↓↓↓↓
	commonData_->modelHandle_[int(ModelType::Bunny)] = myDirectX_->LoadObjFile("resources/subject", "bunny.obj");
	commonData_->modelHandle_[int(ModelType::MultiMaterial)] = myDirectX_->LoadObjFile("resources/subject", "multiMaterial.obj");
	commonData_->modelHandle_[int(ModelType::MultiMesh)] = myDirectX_->LoadObjFile("resources/subject", "multiMesh.obj");
	commonData_->modelHandle_[int(ModelType::Suzanne)] = myDirectX_->LoadObjFile("resources/subject", "suzanne.obj");
	commonData_->modelHandle_[int(ModelType::TeaPot)] = myDirectX_->LoadObjFile("resources/subject", "teapot.obj");
	//↑↑↑↑↑↑↑↑↑↑↑↑↑↑読み込みたいモデル↑↑↑↑↑↑↑↑↑↑↑↑↑

	//↓↓↓↓↓↓↓↓↓↓↓↓↓↓描画したい量(Modelはロードした時点で一つ自動で作られてるヨ!!)↓↓↓↓↓↓↓↓↓↓↓↓↓
	myDirectX_->CreateDrawResource(MyDirectX::kSprite, 100);
	myDirectX_->CreateDrawResource(MyDirectX::kLine, 1000);
	//↑↑↑↑↑↑↑↑↑↑↑↑↑↑描画したい量↑↑↑↑↑↑↑↑↑↑↑↑↑

	//最初のシーンを挿入
	scene_ = std::make_unique<GameScene>(commonData_);
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
