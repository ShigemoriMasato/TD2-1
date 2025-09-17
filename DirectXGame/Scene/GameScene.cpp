#include "GameScene.h"
#include "../Engine/Input/Input.h"
#include "../externals/imgui/imgui.h"
#include "../Engine/Math/MyMath.h"
#include "../Engine/Sound/Sound.h"

using namespace Matrix;

GameScene::GameScene(std::shared_ptr<CommonData> commonData) : Scene(commonData),
persCamera_(new Camera()),
debugCamera_(new DebugCamera()),
orthoCamera_(new Camera()) {
	isDebugCamera = true;

	gridMaker_ = std::make_unique<GridMaker>(debugCamera_, true);
}

GameScene::~GameScene() {
	delete persCamera_;
	delete debugCamera_;
	delete orthoCamera_;
}

void GameScene::Initialize() {
	gridMaker_->Initialize();
	debugCamera_->Initialize();

	persCamera_->SetProjectionMatrix(PerspectiveFovDesc());

	orthoCamera_->SetProjectionMatrix(OrthographicDesc());
	orthoCamera_->MakeMatrix();

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

	if (isDebugCamera) {
		debugCamera_->Update();
		*persCamera_ = *debugCamera_;
	}

#pragma region Directional Light

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

#pragma endregion

#pragma region UVTransform

	ImGui::Begin("UVTransform");
	ImGui::DragFloat2("UVPos", &uvPos_.x, 0.01f);
	ImGui::DragFloat2("UVScale", &uvScale_.x, 0.01f, 0.01f, 10.0f);
	ImGui::DragFloat("UVRotate", &uvRotate_, 0.01f, -3.14f, 3.14f);
	ImGui::End();

	Transform uvTransform{};
	uvTransform.position = { uvPos_.x, uvPos_.y, 0.0f };
	uvTransform.scale = { uvScale_.x, uvScale_.y, 1.0f };
	uvTransform.rotation.z = uvRotate_;

	spriteMaterial_.uvTransform = MakeAffineMatrix(uvTransform);

#pragma endregion

#pragma region Sound

	ImGui::Begin("Sound");
	if (ImGui::Button("Play")) {
		Sound::bgm[0] = true;
	}
	if (ImGui::Button("Stop")) {
		Sound::bgm[0] = false;
	}
	ImGui::Text("Press Play to Repeat Playback");
	ImGui::End();

#pragma endregion

#pragma region XBoxController

	ImGui::Begin("XBoxController");

	Vector2 rightStick = Input::GetXBoxStickState(0);
	Vector2 leftStick = Input::GetXBoxStickState(1);
	ImGui::Text("LeftStick: (%.2f, %.2f)", leftStick.x, leftStick.y);
	ImGui::Text("RightStick: (%.2f, %.2f)", rightStick.x, rightStick.y);

	ImGui::Text("A:%d, B:%d, X:%d, Y:%d", 
		Input::GetXBoxButtonState(XBoxController::kA), Input::GetXBoxButtonState(XBoxController::kB),
		Input::GetXBoxButtonState(XBoxController::kX), Input::GetXBoxButtonState(XBoxController::kY));

	ImGui::Text("Up:%d, Down:%d, Left:%d, Right:%d",
		Input::GetXBoxButtonState(XBoxController::kUp), Input::GetXBoxButtonState(XBoxController::kDown),
		Input::GetXBoxButtonState(XBoxController::kLeft), Input::GetXBoxButtonState(XBoxController::kRight));

	ImGui::End();

#pragma endregion

#pragma region SRT

	ImGui::Begin("SRT");
	ImGui::DragFloat3("Scale", &transform_.scale.x, 0.01f, 0.01f, 10.0f);
	ImGui::DragFloat3("Rotation", &transform_.rotation.x, 0.01f, -3.14f, 3.14f);
	ImGui::DragFloat3("Position", &transform_.position.x, 0.01f);
	ImGui::End();

#pragma endregion

	gridMaker_->Update();

	return nullptr;
}

void GameScene::Draw() const {
	gridMaker_->Draw();

	for (int i = 0; i < int(ModelType::ModelCount); ++i) {
		Render::DrawModel(commonData_->modelHandle_[i], MakeTranslationMatrix(modelPositions_[i]), persCamera_, material_, dLight_);
	}

	Render::DrawSprite(
		{ 0.0f, 0.0f, 0.0f, 1.0f },
		{ 128.0f, 0.0f, 0.0f, 1.0f },
		{ 0.0f, 128.0f, 0.0f, 1.0f },
		{ 128.0f, 128.0f, 0.0f, 1.0f },
		MakeIdentity4x4(), orthoCamera_, spriteMaterial_, {}, UVCHECKER);

	Render::DrawSprite(MakeAffineMatrix(transform_), persCamera_, material_, dLight_, commonData_->textureHandle_[int(TextureType::SRT)]);

	Render::DrawSphere(2.0f, MakeTranslationMatrix({ 2.5f, 7.5f, 0.0f }), persCamera_, material_, dLight_, UVCHECKER);
}
