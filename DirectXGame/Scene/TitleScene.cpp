#include "TitleScene.h"
#include "GameScene.h"
#include "../Engine/Input/Input.h"
#include "../Engine/Camera/Camera.h"
#include "../Engine/Math/MyMath.h"
#include <numbers>

using namespace MyMath;

TitleScene::TitleScene(std::shared_ptr<CommonData> commonData) : Scene(commonData),
camera_(new Camera()) {
	debugCamera_ = new DebugCamera();
	gridMaker_ = new GridMaker(debugCamera_, true);
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

std::unique_ptr<Scene> TitleScene::Update() {
	debugCamera_->Update();
	gridMaker_->Update();

	return nullptr;
}

void TitleScene::Draw() const {
	gridMaker_->Draw();
}
