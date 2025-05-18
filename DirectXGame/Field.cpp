#include "Field.h"
#include <cmath>

Field::Field() {
	const float pie = 3.14159265358f;
	for (int i = 0; i < kCircleNum_; ++i) {
		circles_.push_back(Circle());
	}
}

void Field::Initialize(Matrix4x4* vpMatirx, DirectionalLightData* dLightData) {
	vpMatrix_ = vpMatirx;
	dLightData_ = dLightData;
	frame_ = 140;

	for (int i = 0; i < kCircleNum_; ++i) {
		circles_[i].Initialize(vpMatirx, dLightData, 32);
		if (i % 2 == 0) {
			circles_[i].SetMaterial({ 0.03f, 0.03f, 0.3f, 1.0f, 1 });
		} else {
			circles_[i].SetMaterial({ 0.8f, 0.8f, 0.8f, 1.0f, 1 });
		}
	}
}

void Field::Update() {
	const float pie = 3.14159265258f;
	++frame_;
	if (frame_ >= 240) {
		frame_ -= 240;
	}

	for (int i = 0; i < kCircleNum_; ++i) {
		theta_ = pie * float(frame_) / 180;
		if (frame_ > 180) {
			theta_ = 0.0f;
		}
		Transform tf = { 1.0f, 1.0f, 1.0f };
		tf.position.y = -2.0f + 0.1f * i;
		circles_[i].Update(tf, fabsf(sinf(theta_)) * 4.0f * (i + 1) / kCircleNum_, 0.3f);
	}
}

void Field::Draw(MyDirectX* md) {
	for (int i = 0; i < kCircleNum_; ++i) {
		circles_[i].Draw(md);
	}
}
