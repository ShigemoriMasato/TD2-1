#include "Wire.h"

void Wire::Initialize(ModelData* modelData, Camera* camera) {
	BaseObject::Initialize(modelData, camera);
}

void Wire::Update(float deltaTime) {
	if (isExtending_) {

		Vector3 direction = (targetPos_ - endPos_).Normalize();
		float distance = (targetPos_ - endPos_).Length();
		//現在距離から進ませる
		distance -= extendSpeed_ * deltaTime;

		//距離が0以下になったら終端にする
		if (distance <= 0.0f) {
			distance = 0.0f;
			isExtending_ = false;
		}

		endPos_ = *startPos_ + direction * (initDistance_ - distance);
	}

	if (isVisible_) {
		endPos_ = *startPos_;
	}

	transform_.position = (*startPos_ + endPos_) / 2.0f;
}

void Wire::SetEndPosition(const Vector3& endPos) {
	//目標座標を設定
	targetPos_ = endPos;
	//startからendまでの距離
	initDistance_ = (endPos - *startPos_).Length();
	//伸ばし始める
	isExtending_ = true;
}

bool Wire::Extended() {
	return !isExtending_;
}

void Wire::Shrinked() {
	isVisible_ = false;
}
