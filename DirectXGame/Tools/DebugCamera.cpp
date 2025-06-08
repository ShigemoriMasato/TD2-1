#include "DebugCamera.h"
#include "Input.h"
#include "Camera.h"

using namespace Matrix;

void DebugCamera::Initialize(Camera* camera) {
	if (camera) {
		this->camera_ = *camera;
	}

	center_ = {};

	transform_.scale = Vector3(1.0f, 1.0f, 1.0f);
}

void DebugCamera::Update() {

	Vector3 cameraVelocity{};
	Vector3 centerVelocity{};

	//===================
	//親の移動
	//===================
	if (Input::GetMouseButtonState()[2]) {

		Vector2 mouseMove = Input::GetMouseMove();

		//LSHIFTで移動
		if (Input::GetKeyState()[DIK_LSHIFT]) {

			centerVelocity = Vector3(-mouseMove.x, mouseMove.y, 0.0f) * speed_;

		}
		//なにも押してないと視点を回転
		else {

			transform_.rotation += Vector3(mouseMove.y, mouseMove.x, 0.0f) * speed_ * 0.1f;

		}
	}

	centerVelocity.z += Input::GetMouseWheel() * speed_ * 0.1f;

	transform_.position += centerVelocity * MakeRotationMatrix(transform_.rotation);

	//===================
	//子の移動
	//===================
	camera_.SetTransform(MakeRotationMatrix(transform_.rotation) * MakeTranslationMatrix(transform_.position));
	camera_.SetProjectionMatrix(PerspectiveFovDesc());
}

void DebugCamera::Draw(Render* render, Camera* camera) {
}

Camera DebugCamera::GetCamera() {
	return camera_;
}
