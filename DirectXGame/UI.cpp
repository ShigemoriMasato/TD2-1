#include "UI.h"
#include "Engine/Input/Input.h"
#include "Scene/GameScene.h"
#include "Scene/MotionGraphScene.h"

Button::Button(Camera* camera, Vector3 center) : Object(camera, ShapeType::Cube) {
	transform_->position = center;
	isSelected_ = false;
}

void Button::Initialize() {
}

void Button::Update() {

	transform_->scale = Vector3(0.2f, 0.2f, 0.2f);
	color = 0x222222ff;

	if (isSelected_) {
		transform_->scale = Vector3(1.0f, 1.0f, 1.0f);
		color = 0xffffffff;

		isSelected_ = false;
	}

}

UI::UI(Camera* camera) {
	for (int i = 0; i < buttonNum_; ++i) {
		Vector3 center{};
		center.x = (-buttonNum_ + 1) * 1.0f + i * 2.0f;
		buttons_.push_back(std::make_unique<Button>(camera, center));
	}
}

void UI::Update() {
	BYTE* keys = Input::GetKeyState();
	BYTE* preKeys = Input::GetPreKeyState();
	
	if (keys[DIK_D] && !preKeys[DIK_D]) {
		if (nowSelected_ < buttonNum_ - 1) {
			++nowSelected_;
		}
	}

	if (keys[DIK_A] && !preKeys[DIK_A]) {
		if (nowSelected_ > 0) {
			--nowSelected_;
		}
	}

	for (auto& b : buttons_) {
		b->Update();
	}

	buttons_[nowSelected_]->SetSelected(true);

}

void UI::Draw() {
	for (const auto& b : buttons_) {
		b->Draw();
	}
}

std::unique_ptr<Scene> UI::GetSelectedScene(std::shared_ptr<CommonData> cd) {

	if (Input::GetKeyState(DIK_SPACE) && !Input::GetPreKeyState(DIK_SPACE)) {

		switch (nowSelected_) {
		case BPMMasure:
			return std::make_unique<GameScene>(cd);
			break;
		case MotionGraph:
			return std::make_unique<MotionGraphScene>(cd);
			break;
		}

	}

	return nullptr;

}
