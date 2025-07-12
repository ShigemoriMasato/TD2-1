#include "GameScene.h"
#include "../Engine/Input/Input.h"
#include "../externals/imgui/imgui.h"
#include "../Engine/Math/MyMath.h"
#include "TitleScene.h"

using namespace Matrix;

GameScene::GameScene(std::shared_ptr<CommonData> commonData) : Scene(commonData),
camera_(new Camera()) {
	cameraTrans_ = std::make_unique<Transform>();
}

GameScene::~GameScene() {
	delete camera_;
}

void GameScene::Initialize() {
	*cameraTrans_ = Transform();
	cameraTrans_->position = { 0.0f, 0.0f, -20.0f };
	camera_->SetTransform(cameraTrans_.get());
	camera_->SetProjectionMatrix(PerspectiveFovDesc());
	camera_->MakeMatrix();
}

std::unique_ptr<Scene> GameScene::Update() {

	commonData_->beatManager_->Update();
	commonData_->beatManager_->ImGuiDraw();

	if (Input::GetKeyState(DIK_B)) {
		return std::make_unique<TitleScene>(commonData_);
	}

	return nullptr;
}

void GameScene::Draw() const {
	commonData_->beatManager_->DrawWave(camera_);
}
