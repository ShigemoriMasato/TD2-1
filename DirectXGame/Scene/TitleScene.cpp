#include "TitleScene.h"
#include "GameScene.h"
#include "../Engine/Input/Input.h"
#include "../Engine/Camera/Camera.h"
#include "../Engine/Math/MyMath.h"

using namespace MyMath;

TitleScene::TitleScene(std::shared_ptr<CommonData> commonData) : Scene(commonData) {
	camera_ = new Camera;
}

TitleScene::~TitleScene() {
	delete camera_;
}

void TitleScene::Initialize() {
	camera_->SetProjectionMatrix(PerspectiveFovDesc());
	camera_->SetTransform(cameraTrans_);
	cameraTrans_->scale = { 1.0f, 1.0f, 1.0f };
	cameraTrans_->position = { 0.0f, 0.0f, -20.0f };
	camera_->MakeMatrix();
}

std::unique_ptr<Scene> TitleScene::Update() {

	return nullptr;
}

void TitleScene::Draw() const {

}
