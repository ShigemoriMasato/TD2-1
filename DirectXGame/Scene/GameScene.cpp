#include "GameScene.h"
#include "../Engine/Input/Input.h"
#include "../externals/imgui/imgui.h"
#include "../Engine/Math/MyMath.h"

using namespace Matrix;

GameScene::GameScene(std::shared_ptr<CommonData> commonData) : Scene(commonData),
camera_(new Camera()),
debugCamera_(new DebugCamera()) {
	isDebugCamera = true;

	gridMaker_ = std::make_unique<GridMaker>(debugCamera_, true);
}

GameScene::~GameScene() {
	delete camera_;
	delete debugCamera_;
}

void GameScene::Initialize() {
	gridMaker_->Initialize();
	debugCamera_->Initialize();
	camera_->SetProjectionMatrix(PerspectiveFovDesc());

	for (int i = 0; i < int(ModelType::ModelCount); ++i) {
		Vector3 offset = { -2.5f, 1.5f, 0.0f };
		modelPositions_[i] = Vector3(
			i % 2 * 5.0f,
			float(int(i / 2) * 3.0f),
			0.0f
		) + offset;
	}

}

std::unique_ptr<Scene> GameScene::Update() {

	if (Input::GetKeyState(DIK_R)) {
		Initialize();
	}

	ImGui::Begin("Camera");
	ImGui::Checkbox("Debug Camera", &isDebugCamera);
	ImGui::End();

	if (isDebugCamera) {
		debugCamera_->Update();
		*camera_ = *debugCamera_;
	}

	ImGui::Begin("DLightData");
	ImGui::ColorEdit4("Color", &dLight_.color.x);
	ImGui::DragFloat("Intensity", &dLight_.intensity, 0.01f, 0.0f, 10.0f);
	ImGui::SliderFloat3("Direction", &dLight_.direction.x, -1.0f, 1.0f);
	ImGui::Text("LightKind");
	if (ImGui::Button("NotUseLight")) {
		material_.enableLighting = 0;
	}
	if (ImGui::Button("LambertReflectance")) {
		material_.enableLighting = 1;
	}
	if (ImGui::Button("HalfLambert")) {
		material_.enableLighting = 2;
	}

	ImGui::End();

	dLight_.direction = dLight_.direction.Normalize();

	gridMaker_->Update();

	return nullptr;
}

void GameScene::Draw() const {
	gridMaker_->Draw();

	for (int i = 0; i < int(ModelType::ModelCount); ++i) {
		Render::DrawModel(commonData_->modelHandle_[i], MakeTranslationMatrix(modelPositions_[i]), camera_, material_, dLight_);
	}
}
