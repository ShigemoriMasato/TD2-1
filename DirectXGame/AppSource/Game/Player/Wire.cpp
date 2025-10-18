#include "Wire.h"

void Wire::Initialize(ModelData* modelData, Camera* camera) {
	BaseObject::Initialize(modelData, camera);
	drawResource_ = std::make_unique<DrawResource>();
	drawResource_->Initialize(2);
	drawResource_->camera_ = camera;
	drawResource_->psoConfig_.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
}

void Wire::Update(float deltaTime) {
	if (isExtending_) {

		Vector3 direction = (targetPos_ - endPos_).Normalize();
		float leftDistance = (targetPos_ - endPos_).Length();
		//現在距離から進ませる
		leftDistance -= extendSpeed_ * deltaTime;

		//距離が0.1以下になったら伸ばし切ったこととする6
		if (leftDistance <= 0.1f) {
			endPos_ = targetPos_;
			isExtending_ = false;
		}

		endPos_ = *startPos_ + direction * (initDistance_ - leftDistance);
	}

	if (!isVisible_) {
		endPos_ = *startPos_;
	}

}

void Wire::Draw(Render* render) {
	drawResource_->localPos_ = { *startPos_, endPos_ };
	render->Draw(drawResource_.get());
}

void Wire::SetEndPosition(const Vector3& endPos) {
	//目標座標を設定
	targetPos_ = endPos;
	//startからendまでの距離
	initDistance_ = (endPos - *startPos_).Length();
	//伸ばし始める
	isExtending_ = true;
	//可視化させる
	isVisible_ = true;
}

bool Wire::Extended() {
	return !isExtending_;
}

void Wire::Shrinked() {
	isVisible_ = false;
}
