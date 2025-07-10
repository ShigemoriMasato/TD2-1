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

	ImGui::Begin("Debug Tutorial");
	ImGui::Text("This is a test of a Spherical Camera");
	ImGui::Text("Press LCLICK to move Camera's Rotation");
	ImGui::Text("Roll MouseWheel to move Camera's Distance");
	ImGui::Text("Press LSHIFT to move the camera center");
	ImGui::Text("Press Space to go to Test of Spherical Coordinate");
	ImGui::End();

	Transform a = debugCamera_->GetTransform();

	ImGui::Begin("Camera");
	ImGui::DragFloat3("Position", &transform_.position.x, 0.1f);
	ImGui::DragFloat3("Rotation", &transform_.rotation.x, 0.1f);
	ImGui::DragFloat3("Scale", &transform_.scale.x, 0.1f);
	ImGui::Checkbox("Debug Camera", &isDebugCamera_);
	ImGui::End();

	ImGui::Begin("Debug Camera");
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", a.position.x, a.position.y, a.position.z);
	ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", a.rotation.x, a.rotation.y, a.rotation.z);
	ImGui::Text("Scale: (%.2f, %.2f, %.2f)", a.scale.x, a.scale.y, a.scale.z);
	ImGui::End();

	if (isDebugCamera_) {
		debugCamera_->Update();
	} else {
		camera_->SetTransform(transform_);
		camera_->MakeMatrix();
	}

	if(Input::GetKeyState(DIK_F1) && !Input::GetPreKeyState(DIK_F1)) {
		isDebugCamera_ = !isDebugCamera_;
	}

	return nullptr;
}

void GameScene::Draw() const {
	
	Transform t{};

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < 3; ++k) {
				if (i != 1 && j != 1 && k != 1) {
					t.position = { float(i) - 1.0f, float(j) - 1.0f, float(k) - 1.0f };
					Render::DrawBox(MakeAffineMatrix(t), camera_, {}, {}, 0);
				}
			}
		}
	}

}
