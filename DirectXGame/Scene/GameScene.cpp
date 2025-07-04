#include "GameScene.h"
#include "../Engine/Input/Input.h"
#include "../externals/imgui/imgui.h"
#include "../Engine/Math/MyMath.h"

using namespace Matrix;

GameScene::GameScene(std::shared_ptr<CommonData> commonData) : Scene(commonData),
camera_(new Camera()),
debugCamera_(new DebugCamera()) {
}

GameScene::~GameScene() {
	delete camera_;
	delete debugCamera_;
}

void GameScene::Initialize() {
	debugCamera_->Initialize();
	camera_->SetProjectionMatrix(PerspectiveFovDesc());
}

std::unique_ptr<Scene> GameScene::Update() {

	ImGui::Begin("Camera");
	ImGui::Checkbox("Debug Camera", &isDebugCamera);
	ImGui::End();

	if (isDebugCamera) {
		debugCamera_->Update();
		*camera_ = debugCamera_->GetCamera();
	}
	return nullptr;
}

void GameScene::Draw() const {
}
