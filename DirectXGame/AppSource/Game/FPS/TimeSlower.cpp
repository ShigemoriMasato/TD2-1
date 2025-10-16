#include "TimeSlower.h"
#include <Math/MyMath.h>

using namespace MyMath;

TimeSlower::TimeSlower(FPSObserver* fps) {
	fps_ = fps;
}

TimeSlower::~TimeSlower() {
}

void TimeSlower::Update() {
	float trueDelta = fps_->GetDeltatime();

	//スロウモーションにする
	if (timer_ > 0.0f) {
		timer_ -= trueDelta;

		if (timer_ <= 0.0f) {
			timer_ = 0.0f;
		}
	}

	deltaTime_ = trueDelta * timeRate_;

	if (timer_ < calculateTime_) {
		CalculateDeltaTime();
	}
}

void TimeSlower::StartSlow(float time) {
	timer_ = time;
	timeRate_ = slowRate_;
}

void TimeSlower::EndSlow(bool calculate) {
	calculate ? timer_ = 0.0f : timer_ = calculateTime_;
}

void TimeSlower::CalculateDeltaTime() {
	//補完をする
	//現在はとりあえずただの線形補完を行う

	float t = (calculateTime_ - timer_) / calculateTime_;
	timeRate_ = lerp(slowRate_, 1.0f, t);
}
