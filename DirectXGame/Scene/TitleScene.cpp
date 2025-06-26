#include "TitleScene.h"
#include "GameScene.h"
#include "../Engine/Input/Input.h"
#include "../Engine/Camera/Camera.h"
#include "../Engine/Math/MyMath.h"

using namespace MyMath;

TitleScene::TitleScene(std::shared_ptr<CommonData> commonData) : Scene(commonData),
camera_(new Camera()),
debugCamera(new DebugCamera()){
}

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {
	camera_->SetProjectionMatrix(PerspectiveFovDesc());
	Transform cameraTransform{};
	cameraTransform.position = { 0.0f, 0.0f, -10.0f };
	camera_->SetTransform(cameraTransform);
	camera_->MakeMatrix();
	debugCamera->Initialize();
}

Scene* TitleScene::Update() {

	debugCamera->Update();
	*camera_ = debugCamera->GetCamera();

	return nullptr;
}

void TitleScene::Draw() const {

	Render::DrawBox(MakeAffineMatrix(Transform()), camera_, {}, {}, 0);

}
