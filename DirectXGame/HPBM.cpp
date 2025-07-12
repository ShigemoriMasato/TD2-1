#include "HPBM.h"
#include "Engine/Input/Input.h"
#include "externals/imgui/imgui.h"
#include "Engine/Sound/Sound.h"
#include <numbers>

using namespace HPBM;

Effect::Effect(Camera* camera, Vector3 center) : Object(camera, ShapeType::Cube) {
	transform_->position = center;
	transform_->scale = Vector3(1.0f, 0.4f, 0.2f);
}

void HPBM::Effect::Update() {
	lifeTime_++;
	if (lifeTime_ >= kMaxLifeTime) {
		// Effectの寿命が尽きたら削除
		isActive_ = false;
	}

	float alpha = MyMath::lerp(0.5f, 0.0f, static_cast<float>(lifeTime_) / kMaxLifeTime);
	color = color & 0xffffff00 | static_cast<uint32_t>(alpha * 255.0f);
	transform_->scale *= 1.03f;
}

Button::Button(Camera* camera) : Object(camera, ShapeType::Cube) {
	Initialize();
}

void Button::Initialize() {
	transform_->scale = Vector3(1.0f, 0.4f, 0.2f);
	transform_->position.y = -2.5f;
	effects_.clear();
}

void Button::Update() {
	if (Input::GetKeyState(DIK_SPACE) && !Input::GetPreKeyState(DIK_SPACE)) {
		effects_.push_back(std::make_shared<Effect>(camera_, transform_->position));
	}

	for (int i = 0; i < effects_.size(); ++i) {
		effects_[i]->Update();
		if (!effects_[i]->GetIsActive()) {
			effects_.erase(effects_.begin() + i--);
		}
	}
}

void HPBM::Button::Draw(const Matrix4x4* worldMatrix) const {
	Object::Draw(worldMatrix);

	for (auto button : effects_) {
		button->SetCamera(camera_);
		button->Draw(worldMatrix);
	}
}

HPBM::Measure::Measure() {
	button_ = std::make_unique<Button>(nullptr);
}

HPBM::Measure::~Measure() {
}

void HPBM::Measure::Update() {

	ImGui::SetNextWindowSize({ 150, 720 });
	ImGui::SetNextWindowPos({ 200, 0 });
	ImGui::Begin("Gap Buffer");
	for (auto& g : gaps_) {
		ImGui::Text("Gap: %f", g);
	}
	ImGui::End();

	if (isMeasuring_) {

		if (Input::GetKeyState(DIK_SPACE) && !Input::GetPreKeyState(DIK_SPACE)) {
			if (!isActiveSpace_) {
				isActiveSpace_ = true;
			} else {
				gaps_.push_back(gap_);
			}

			gap_ = 0.0f;
		}

		if (Input::GetKeyState(DIK_RETURN)) {
			isMeasuring_ = false;
		}

		if (isActiveSpace_) {
			frame_++;
			gap_ += deltatime;
		}

	} else {

		if (isActiveSpace_) {
			isActiveSpace_ = false;

			float sum = 0.0f;
			for (float g : gaps_) {
				sum += g;
			}

			bpm_ = (1.0f / (sum / gaps_.size())) * 60.0f;
		}

	}

	button_->Update();
}

void HPBM::Measure::Draw(Camera* camera) const {
	button_->SetCamera(camera);
	button_->Draw();
}

void HPBM::Measure::MeasureStart(int index) {
	Sound::bgm[index] = true;
	isMeasuring_ = true;

	frame_ = 0;
	gaps_.clear();
	gap_ = 0.0f;
	bpm_ = -1.0f;

	button_->Initialize();
}

float HPBM::Measure::GetBPM() const {
	if (isMeasuring_) {
		return -1.0f; // 測定中はBPMを返さない
	}
	return bpm_;
}
