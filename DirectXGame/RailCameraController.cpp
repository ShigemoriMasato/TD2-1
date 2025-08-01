#include "RailCameraController.h"
#include "externals/imgui/imgui.h"
#include "Engine/Render/Render.h"

RailCameraController::RailCameraController(Camera* camera) :
transform_(std::make_unique<Transform>()),
camera_(std::make_shared<Camera>()) {
	if (camera) {
		*camera_ = *camera;
	}
	camera_->SetTransform(transform_.get());
}

void RailCameraController::Initialize() {

	//todo targetとlaptimeを外部から決められるようにする
	transform_->position = { 0.0f, 0.0f, 0.0f };
	transform_->rotation = { 0.0f, 0.0f, 0.0f };
	camera_->SetProjectionMatrix(PerspectiveFovDesc());
}

void RailCameraController::Update() {
	ImGui::Begin("Camera");
	ImGui::DragFloat3("Position", &transform_->position.x, 0.1f);
	ImGui::DragFloat3("Rotation", &transform_->rotation.x, 0.01f);
	ImGui::End();

	++lapTimer_;
	
	if (lapTimer_ > lapTime_) {
		lapTimer_ = 0;
		t = 0.0f;
	}

	t = static_cast<float>(lapTimer_) / lapTime_;

	transform_->position = targetPos_ * t;

	camera_->MakeMatrix();
}

void RailCameraController::Draw(Camera* camera) {

	for (int i = 0; i < catmullPoints_.size() - 1; ++i) {
		Render::DrawLine(MyMath::ConvertVector(catmullPoints_[i], 1.0f), MyMath::ConvertVector(catmullPoints_[i + 1], 1.0f),
			MakeIdentity4x4(), camera, {1.0f, 0.0f, 0.0f, 1.0f});
	}

}

