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
	commonData_->modelHandle_[int(ModelType::Player)] = myDirectX_->LoadObjFile("Resources/Player", "Plane.obj");
	commonData_->modelHandle_[int(ModelType::SkySphere)] = myDirectX_->LoadObjFile("Resources/SkySphere", "sky_sphere.obj");
	commonData_->modelHandle_[int(ModelType::Bullet)] = myDirectX_->LoadObjFile("Resources/Bullet", "Bullet.obj");
	commonData_->modelHandle_[int(ModelType::AccelerateGate)] = myDirectX_->LoadObjFile("Resources/AccelerateGate", "AccelerateGate.obj");
	commonData_->modelHandle_[int(ModelType::lowCommet)] = myDirectX_->LoadObjFile("Resources/Commet", "Commet1.obj");
	commonData_->modelHandle_[int(ModelType::middleCommet)] = myDirectX_->LoadObjFile("Resources/Commet", "Commet2.obj");
	commonData_->modelHandle_[int(ModelType::highCommet)] = myDirectX_->LoadObjFile("Resources/Commet", "Commet3.obj");
	commonData_->modelHandle_[int(ModelType::Title)] = myDirectX_->LoadObjFile("Resources/Title", "Title.obj");
	commonData_->modelHandle_[int(ModelType::C)] = myDirectX_->LoadObjFile("Resources/C", "C.obj");
	commonData_->modelHandle_[int(ModelType::l)] = myDirectX_->LoadObjFile("Resources/l", "l.obj");
	commonData_->modelHandle_[int(ModelType::e)] = myDirectX_->LoadObjFile("Resources/e", "e.obj");
	commonData_->modelHandle_[int(ModelType::a)] = myDirectX_->LoadObjFile("Resources/a", "a.obj");
	commonData_->modelHandle_[int(ModelType::r)] = myDirectX_->LoadObjFile("Resources/r", "r.obj");
	commonData_->modelHandle_[int(ModelType::Bikkuri)] = myDirectX_->LoadObjFile("Resources/Bikkuri", "Bikkuri.obj");
	//↑↑↑↑↑↑↑↑↑↑↑↑↑↑読み込みたいモデル↑↑↑↑↑↑↑↑↑↑↑↑↑

	//↓↓↓↓↓↓↓↓↓↓↓↓↓↓描画したい量↓↓↓↓↓↓↓↓↓↓↓↓↓
	myDirectX_->CreateDrawResource(MyDirectX::kBox, 100);
	myDirectX_->CreateDrawResource(MyDirectX::kSprite, 200);
	myDirectX_->CreateDrawResource(MyDirectX::kLine, 1000);
	myDirectX_->CreateDrawResource(MyDirectX::kSphere, 1000);
	myDirectX_->CreateModelDrawResource(commonData_->modelHandle_[int(ModelType::Player)], 10);
	myDirectX_->CreateModelDrawResource(commonData_->modelHandle_[int(ModelType::Enemy)], 50);
	myDirectX_->CreateModelDrawResource(commonData_->modelHandle_[int(ModelType::Bullet)], 100);
	myDirectX_->CreateModelDrawResource(commonData_->modelHandle_[int(ModelType::AccelerateGate)], 50);
	myDirectX_->CreateModelDrawResource(commonData_->modelHandle_[int(ModelType::lowCommet)], 100);
	myDirectX_->CreateModelDrawResource(commonData_->modelHandle_[int(ModelType::Title)], 1);
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

			//前フレームの実行時間を計測
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - executeTime_);

			frameTimes_.push_back(static_cast<int>(1.0f / (duration.count() / 1000.0f)));

			executeTime_ = std::chrono::high_resolution_clock::now();

			return true;
		}

	}

	//ウィンドウのxボタンが押されたらfalseを返す
	return false;
}

void SceneManager::Update() {

	//Sceneの切り替え
	if (nextScene_ != nullptr && scene_.get() != nextScene_.get()) {
		scene_.reset();
		scene_ = std::move(nextScene_);
		scene_->Initialize();
	}

	myDirectX_->BeginFrame();

	input_->Update();

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
