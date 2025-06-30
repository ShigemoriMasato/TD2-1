#include "DebugCamera.h"
#include "../Input/Input.h"
#include "Camera.h"

using namespace Matrix;

void DebugCamera::Initialize(Camera* camera) {
	if (camera) {
		this->camera_ = *camera;
	}

	center_ = {};

	transform_.scale = Vector3(1.0f, 1.0f, 1.0f);
	transform_.position = Vector3(0.0f, 0.0f, -20.0f);

	spherical_.x = 20.0f;
	spherical_.y = 1.57f;
	spherical_.z = -1.57f;

}

void DebugCamera::Update() {

	Vector3 cameraVelocity{};
	Vector3 centerVelocity{};

	//===================
	//球面座標系
	//===================
	Vector2 mouseMove{};
	if (Input::GetMouseButtonState()[0]) {
		mouseMove = Input::GetMouseMove();
		mouseMove.y *= -1.0f; // Y軸を反転
	}
	float mouseWheel = -Input::GetMouseWheel();

	if (Input::GetKeyState(DIK_LSHIFT)) {
		center_ += Vector3(-mouseMove.x * speed_, mouseMove.y * speed_, mouseWheel * 0.05f) * MakeRotationMatrix(transform_.rotation);
	} else {
		spherical_ += Vector3(mouseWheel * 0.05f, mouseMove.y * speed_, -mouseMove.x * speed_);
	}

	spherical_.x = max(0.01f, spherical_.x); // マイナスにならないようにする

	transform_.position.x = 0.0f;
	transform_.position.y = 0.0f;
	transform_.position.z = -spherical_.x;

	ImGui::Begin("Debug Camera");
	ImGui::Checkbox("Adjust Rotation", &adjustRotation);
	ImGui::Text("mouse move : (%.2f, %.2f)", mouseMove.x, mouseMove.y);
	ImGui::DragFloat3("rotation", &transform_.rotation.x, 0.01f);
	ImGui::Text("spherical: (%f, %f, %f)", spherical_.x, spherical_.y, spherical_.z);
	ImGui::End();

	if (adjustRotation) {
		transform_.rotation = {
			(spherical_.y - 1.57f),
			(spherical_.z + 1.57f),
			0.0f // Z軸回転はなし
		};
	}

	//===================
	//座標の適用
	//===================
	camera_.SetTransform(MakeTranslationMatrix(center_) * MakeScaleMatrix(transform_.scale) * MakeRotationMatrix(transform_.rotation) * Inverse(MakeTranslationMatrix(transform_.position)));
	camera_.SetProjectionMatrix(PerspectiveFovDesc());
	camera_.MakeMatrix();
}

void DebugCamera::Draw(Render* render, Camera* camera) {
}

Camera DebugCamera::GetCamera() {
	return camera_;
}

Transform DebugCamera::GetTransform() {
	return transform_;
}
