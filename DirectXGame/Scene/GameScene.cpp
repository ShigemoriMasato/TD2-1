#include "GameScene.h"
#include "../Engine/Input/Input.h"
#include "../externals/imgui/imgui.h"
#include "../Engine/Math/MyMath.h"
#include "TitleScene.h"

using namespace Matrix;

GameScene::GameScene(std::shared_ptr<CommonData> commonData) : Scene(commonData) {
	camera_ = new Camera();
	debugCamera_ = new DebugCamera();
}

GameScene::~GameScene() {
	delete camera_;
	delete debugCamera_;
}

void GameScene::Initialize() {
	debugCamera_->Initialize();
	camera_->SetProjectionMatrix(PerspectiveFovDesc());

	isDebugCamera_ = true;
}

Scene* GameScene::Update() {

	Transform a = debugCamera_->GetTransform();

	ImGui::Begin("Camera");
	ImGui::DragFloat3("Position", &transform_.position.x, 0.1f);
	ImGui::DragFloat3("Rotation", &transform_.rotation.x, 0.1f);
	ImGui::DragFloat3("Scale", &transform_.scale.x, 0.1f);
	ImGui::Checkbox("Debug Camera", &isDebugCamera_);
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", a.position.x, a.position.y, a.position.z);
	ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", a.rotation.x, a.rotation.y, a.rotation.z);
	ImGui::Text("Scale: (%.2f, %.2f, %.2f)", a.scale.x, a.scale.y, a.scale.z);
	ImGui::End();

	if (isDebugCamera_) {
		debugCamera_->Update();
		*camera_ = debugCamera_->GetCamera();
	} else {
		camera_->SetTransform(transform_);
		camera_->MakeMatrix();
	}

	if(Input::GetKeyState(DIK_F1) && !Input::GetPreKeyState(DIK_F1)) {
		isDebugCamera_ = !isDebugCamera_;
	}

	if (Input::GetKeyState(DIK_SPACE) && !Input::GetPreKeyState(DIK_SPACE)) {
		return new TitleScene(commonData_);
	}

	return nullptr;
}

void GameScene::Draw() const {
	
	Render::DrawBox(MakeIdentity4x4(), camera_, {}, {}, 0);

}
