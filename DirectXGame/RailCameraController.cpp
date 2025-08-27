#include "RailCameraController.h"
#include "externals/imgui/imgui.h"
#include "Engine/Render/Render.h"
#include "Scene/GameScene.h"
#include <algorithm>

using namespace Matrix;

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
		{ 0.0f, 3.0f, 0.0f },
		{0.0f, -10.0f, 300.0f},
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

	//全体速度が0.2未満にならないようにする
	effectSpeed_ = std::max(-speed_ + 0.1f, effectSpeed_);

	moveDistance_ += speed_ + effectSpeed_;
	float t = GetTFromDistance(distanceSamples_, moveDistance_);
	if (t >= 1.0f) {
		t = 1.0f;
	}
	transform_->position = GetCatmullPoint(catmullPoints_, t);

	if (t == 1.0f) {
		GameScene::isGoal_ = true;
	}

	camera_->MakeMatrix();

	effectSpeed_ *= attenuation_;

}

void RailCameraController::Draw(Camera* camera) {
#ifdef _DEBUG

	for(int i = 0; i < catmullPoints_.size() - 1; ++i) {
		Render::DrawLine(
			MyMath::ConvertVector(catmullPoints_[i], 1.0f) + Vector4(0.0f, 0.0f, 20.0f),
			MyMath::ConvertVector(catmullPoints_[i + 1], 1.0f) + Vector4(0.0f, 0.0f, 20.0f),
			MakeIdentity4x4(), camera,
			{ 1.0f, 0.0f, 0.0f, 1.0f }
		);
	}

#else

	float currentT = GetTFromDistance(distanceSamples_, moveDistance_);
	int totalSegments = static_cast<int>(catmullPoints_.size()) - 1;

	// 現在のtに対応するインデックスを計算
	int maxIndex = static_cast<int>(currentT * totalSegments);
	maxIndex = std::clamp(maxIndex, 0, totalSegments - 1);

	Render::DrawLine(
		Vector4(transform_->position.x, transform_->position.y, transform_->position.z, 1.0f) + Vector4(0.0f, 0.0f, 20.0f),
		MyMath::ConvertVector(catmullPoints_[maxIndex + 1], 1.0f) + Vector4(0.0f, 0.0f, 20.0f),
		MakeIdentity4x4(), camera,
		{ 1.0f, 0.0f, 0.0f, 1.0f }
	);

	for (int i = maxIndex + 1; i < catmullPoints_.size() - 1; ++i) {
		Render::DrawLine(
			MyMath::ConvertVector(catmullPoints_[i], 1.0f) + Vector4(0.0f, 0.0f, 20.0f),
			MyMath::ConvertVector(catmullPoints_[i + 1], 1.0f) + Vector4(0.0f, 0.0f, 20.0f),
			MakeIdentity4x4(), camera,
			{ 1.0f, 0.0f, 0.0f, 1.0f }
		);
	}

#endif

}
