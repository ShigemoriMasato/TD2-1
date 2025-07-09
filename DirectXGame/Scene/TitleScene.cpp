#include "TitleScene.h"
#include "GameScene.h"
#include "../Engine/Input/Input.h"
#include "../Engine/Camera/Camera.h"
#include "../Engine/Math/MyMath.h"
#include <numbers>

using namespace MyMath;

TitleScene::TitleScene(std::shared_ptr<CommonData> commonData) : Scene(commonData),
camera_(new Camera()) {
	gridMaker_ = new GridMaker(camera_);
	debugCamera_ = new DebugCamera();
}

TitleScene::~TitleScene() {
	delete camera_;
	delete gridMaker_;
}

void TitleScene::Initialize() {
	cameraTransform_.position.z = -15.0f;
	camera_->SetProjectionMatrix(PerspectiveFovDesc());
	debugCamera_->Initialize();
}

Scene* TitleScene::Update() {
	debugCamera_->Update();
	*camera_ = debugCamera_->GetCamera();
	gridMaker_->Update();

	return nullptr;
}

void TitleScene::Draw() const {
	gridMaker_->Draw();
}
