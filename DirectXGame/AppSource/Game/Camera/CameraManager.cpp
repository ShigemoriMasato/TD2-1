#include "CameraManager.h"

namespace {
	Vector3 lerp(Vector3 a, Vector3 b, float t) {
		return MyMath::lerp(a, b, t);
	}
}

CameraManager::CameraManager() {
	camera_ = std::make_unique<Camera>();

	debugCamera_ = std::make_unique<DebugCamera>();
}

void CameraManager::Initialize(Vector3 initPos) {
	camera_->SetProjectionMatrix(PerspectiveFovDesc());
	debugCamera_->Initialize();
	transform_.position = initPos;
}

void CameraManager::Initialize(Vector3* targetPos) {
	camera_->SetProjectionMatrix(PerspectiveFovDesc());
	debugCamera_->Initialize();
	SetForcusTarget(targetPos);
}

void CameraManager::Update(float deltaTime) {
	pcalculate = std::min(pcalculate + deltaTime, 1.0f);
	ocalculate = std::min(ocalculate + deltaTime, 1.0f);
	scalculate = std::min(scalculate + deltaTime, 1.0f);

	const float followSpeed = 8.0f;
	float t = 1.0f - std::exp(-followSpeed * deltaTime);

	transform_.position = lerp(transform_.position, *targetPos_ + targetOffset_, t);
	transform_.scale = lerp(transform_.scale, targetScale_, t);
	offset_ = lerp(offset_, targetOffset_, t);

	camera_->SetTransform(transform_);
	camera_->MakeMatrix();



	if (isDebug_) {
		debugCamera_->Update();
		*camera_ = *debugCamera_;
	}
}

void CameraManager::DrawImGui() {
	ImGui::Begin("Camera");
	ImGui::Checkbox("Debug Camera", &isDebug_);
	ImGui::DragFloat3("Offset", &targetOffset_.x, 0.1f);
	ImGui::DragFloat("Scale", &targetScale_.x, 0.1f, 0.1f, 10.0f);
	ImGui::DragFloat3("Rotate", &transform_.rotation.x, 0.01f);
	ImGui::Text("Position : %.2f, %.2f, %.2f", transform_.position.x, transform_.position.y, transform_.position.z);
	ImGui::End();
}

void CameraManager::FinishCalculation() {
	pcalculate = 1.0f;
	ocalculate = 1.0f;
	scalculate = 1.0f;
	transform_.position = *targetPos_ + targetOffset_;
	transform_.scale = targetScale_;
	offset_ = targetOffset_;
}

Camera* CameraManager::GetCamera() {
	return isDebug_ ? debugCamera_.get() : camera_.get();
}

void CameraManager::SetForcusTarget(Vector3* targetPos) {
	targetPos_ = targetPos;
	previousTargetPos_ = transform_.position;
	pcalculate = 0.0f;
}

void CameraManager::SetOffset(const Vector3& offset) {
	targetOffset_ = offset;
	previousOffset_ = offset_;
	ocalculate = 0.0f;
}

void CameraManager::SetScale(float ratio) {
	targetScale_ = Vector3(ratio, ratio, 1.0f);
	previousScale_ = transform_.scale;
	scalculate = 0.0f;
}

void CameraManager::SetDebug(bool debug) {
	isDebug_ = debug;
}
