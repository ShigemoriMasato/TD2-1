#include "TitleLogo.h"

TitleLogo::TitleLogo(Camera* camera, int handle) : Object(camera, ShapeType::Model) {
	handle_ = handle;
}

TitleLogo::~TitleLogo() {
}

void TitleLogo::Update() {

	++frame_;

	transform_->rotation.y = rotMax_ * sinf(frame_ * rotFreq_);
	transform_->scale.x = scaMaxX_ * (sinf(frame_ * scaFreq_)) + 1.0f;
	transform_->scale.y = -scaMaxY_ * (sinf(frame_ * scaFreq_)) + 1.0f;

	ImGui::Begin("TitleLogo");
	ImGui::DragFloat("Rotation Frequency", &rotFreq_, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Rotation Max", &rotMax_, 0.1f, 0.0f, 360.0f);
	ImGui::DragFloat("Scale Frequency", &scaFreq_, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Scale Max X", &scaMaxX_, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Scale Max Y", &scaMaxY_, 0.01f, 0.0f, 1.0f);
	ImGui::End();

}
