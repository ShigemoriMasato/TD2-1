#include "TitleScene.h"
#include "GameScene.h"
#include "../Engine/Input/Input.h"
#include "../Engine/Camera/Camera.h"
#include "../Engine/Math/MyMath.h"
#include <numbers>

using namespace MyMath;

TitleScene::TitleScene(std::shared_ptr<CommonData> commonData) : Scene(commonData),
camera_(new Camera()) {
}

TitleScene::~TitleScene() {
	delete camera_;
}

void TitleScene::Initialize() {
	cameraTransform_.position.z = -15.0f;
	spherical_.x = 1.5f;
	camera_->SetProjectionMatrix(PerspectiveFovDesc());
}

Scene* TitleScene::Update() {

	ImGui::Begin("Camera");
	ImGui::DragFloat3("position", &cameraTransform_.position.x, 0.05f);
	ImGui::DragFloat3("rotation", &cameraTransform_.rotation.x, 0.05f);
	ImGui::DragFloat3("scale", &cameraTransform_.scale.x, 0.05f);
	ImGui::End();

	camera_->SetTransform(cameraTransform_);
	camera_->MakeMatrix();

	ImGui::Begin("sphere");
	ImGui::DragFloat3("spherical", &spherical_.x, 0.05f);
	ImGui::End();

	ImGui::Begin("Debug Tutorial");
	ImGui::Text("This is a test of a Spherical Coordinate");
	ImGui::Text("Press Space to go to MoveCamera");
	ImGui::End();

	spherical_.x = max(0.01f, spherical_.x); // マイナスにならないようにする

	transform_.position.x = std::sinf(spherical_.y) * std::cosf(spherical_.z);
	transform_.position.y = std::cosf(spherical_.y);
	transform_.position.z = std::sinf(spherical_.y) * std::sinf(spherical_.z);

	transform_.position *= spherical_.x; // 半径を適用

	for (int i = 0; i < 32; i++) {

		float t = static_cast<float>(i) / 32.0f * std::numbers::pi_v<float> * 2; // 0.0f ~ 2*PI
		float next = static_cast<float>(i + 1) / 32.0f * std::numbers::pi_v<float> *2; // NextT

		start_[0][i] = Vector3(
			std::sinf(spherical_.y) * std::cosf(t),
			std::cosf(spherical_.y),
			std::sinf(spherical_.y) * std::sinf(t)
	) * spherical_.x;
		end_[0][i] = Vector3(
			std::sinf(spherical_.y) * std::cosf(next),
			std::cosf(spherical_.y),
			std::sinf(spherical_.y) * std::sinf(next)
		) * spherical_.x;
	};

	for(int i = 0; i < 32; i++) {

		float t = static_cast<float>(i) / 16.0f * std::numbers::pi_v<float>; // 0.0f ~ 2*PI
		float next = static_cast<float>(i + 1) / 16.0f * std::numbers::pi_v<float>; // NextT

		start_[1][i] = Vector3(
			std::sinf(t) * std::cosf(spherical_.z),
			std::cosf(t),
			std::sinf(t) * std::sinf(spherical_.z)
		) * spherical_.x;
		end_[1][i] = Vector3(
			std::sinf(next) * std::cosf(spherical_.z),
			std::cosf(next),
			std::sinf(next) * std::sinf(spherical_.z)
		) * spherical_.x;
	}

	if (Input::GetKeyState(DIK_SPACE) && !Input::GetPreKeyState(DIK_SPACE)) {
		return new GameScene(commonData_);
	}

	return nullptr;
}

void TitleScene::Draw() const {

	Render::DrawBox(MakeAffineMatrix(Transform()), camera_, {}, {}, 0);

	Render::DrawSphere(0.5f, MakeAffineMatrix(transform_), camera_, {}, {}, 1);

	for(int i = 0; i < 32; i++) {
		Render::DrawLine(start_[0][i], end_[0][i], MakeIdentity4x4(), camera_, {}, {}, 1);
	}

	for(int i = 0; i < 32; i++) {
		Render::DrawLine(start_[1][i], end_[1][i], MakeIdentity4x4(), camera_, {1.0f, 0.0f, 0.0f, 1.0f}, {}, 1);
	}

}
