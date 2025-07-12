#include "TitleScene.h"
#include "GameScene.h"
#include "../Engine/Input/Input.h"
#include "../Engine/Camera/Camera.h"
#include "../Engine/Math/MyMath.h"
#include "../Engine/Sound/Sound.h"

using namespace MyMath;

TitleScene::TitleScene(std::shared_ptr<CommonData> commonData) : Scene(commonData) {
	camera_ = new Camera;
	ui_ = new UI(camera_);
}

TitleScene::~TitleScene() {
	delete camera_;
	delete ui_;
}

void TitleScene::Initialize() {
	camera_->SetProjectionMatrix(PerspectiveFovDesc());
	camera_->SetTransform(cameraTrans_);
	cameraTrans_->scale = { 1.0f, 1.0f, 1.0f };
	cameraTrans_->position = { 0.0f, 0.0f, -20.0f };
	camera_->MakeMatrix();
}

std::unique_ptr<Scene> TitleScene::Update() {

	ui_->Update();

	return ui_->GetSelectedScene(commonData_);
}

void TitleScene::Draw() const {
	ui_->Draw();
}
