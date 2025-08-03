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
	transform_->position = { 0.0f, 0.0f, 0.0f };
	transform_->rotation = { 0.0f, 0.0f, 0.0f };
	camera_->SetProjectionMatrix(PerspectiveFovDesc());

	controllPoints_ = { {0.0f, 0.0f, -800.0f},
		{0.0f, 0.0f, -400.0f},
		{ 0.0f, 0.0f, 0.0f },
		{0.0f, -50.0f, 300.0f},
		{0.0f, 0.0f, 600.0f},
		{0.0f, 0.0f, 1000.0f}
	};

	catmullPoints_ = GetCatmullPoints(controllPoints_, 50);

	distanceSamples_ = BuildDistanceTable(catmullPoints_, int(controllPoints_.size()));

	transform_->position = catmullPoints_[0];
}

void RailCameraController::Update() {
	ImGui::Begin("Camera");
	ImGui::DragFloat3("Position", &transform_->position.x, 0.1f);
	ImGui::DragFloat3("Rotation", &transform_->rotation.x, 0.01f);
	ImGui::Text("Speed: %.2f, Effect Speed: %.2f", speed_, effectSpeed_);
	ImGui::End();

	moveDistance_ += speed_ + effectSpeed_;
	float t = GetTFromDistance(distanceSamples_, moveDistance_);
	if (t >= 1.0f) {
		moveDistance_ = 0.0f;
		t = 0.0f;
	}
	transform_->position = GetCatmullPoint(catmullPoints_, t);

	camera_->MakeMatrix();

	effectSpeed_ *= attenuation_;
}

void RailCameraController::Draw(Camera* camera) {

	for (int i = 0; i < catmullPoints_.size() - 1; ++i) {
		Render::DrawLine(MyMath::ConvertVector(catmullPoints_[i], 1.0f) + Vector4(0.0f, 0.0f, 20.0f), MyMath::ConvertVector(catmullPoints_[i + 1], 1.0f) + Vector4(0.0f, 0.0f, 20.0f),
			MakeIdentity4x4(), camera, { 1.0f, 0.0f, 0.0f, 1.0f });
	}

}
