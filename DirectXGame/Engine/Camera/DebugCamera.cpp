#include "DebugCamera.h"
#include "../Input/Input.h"
#include "Camera.h"

using namespace Matrix;

void DebugCamera::Initialize() {
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
	if (Input::GetMouseButtonState()[2]) {
		mouseMove = Input::GetMouseMove();
		mouseMove.y *= -1.0f; // Y軸を反転
	}
	float mouseWheel = -Input::GetMouseWheel();

	if (Input::GetKeyState(DIK_LSHIFT)) {
		center_ += Vector3(mouseMove.x * speed_, mouseMove.y * speed_, mouseWheel * 0.05f) * MakeRotationMatrix(transform_.rotation);
	} else {
		spherical_ += Vector3(mouseWheel * 0.05f, mouseMove.y * speed_, -mouseMove.x * speed_);
	}

	spherical_.x = max(0.01f, spherical_.x); // マイナスにならないようにする

	transform_.position.x = std::sinf(spherical_.y) * std::cosf(spherical_.z);
	transform_.position.y = std::cosf(spherical_.y);
	transform_.position.z = std::sinf(spherical_.y) * std::sinf(spherical_.z);

	transform_.position *= spherical_.x; // 半径を適用

	transform_.rotation = { -spherical_.y + 1.57f, -spherical_.z - 1.57f, 0.0f };

	// Actual camera position in world space
	Vector3 actualCameraPosition = center_ + transform_.position;
	
	//===================
	//座標の適用
	//===================
	SetTransform(MakeTranslationMatrix(center_) * Inverse(MakeTranslationMatrix(transform_.position)) * Inverse(MakeRotationMatrix(transform_.rotation)) * MakeScaleMatrix(transform_.scale));
	SetProjectionMatrix(PerspectiveFovDesc());
	MakeMatrix();
}

Transform DebugCamera::GetTransform() {
	return transform_;
}

Vector3 DebugCamera::GetPosition() const {
	return center_ + transform_.position;
}

Vector3 DebugCamera::GetCenter() const {
	return -center_;
}
